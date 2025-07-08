#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

using namespace std;

class barbero_dormilon
{
public:
    void cortar_pelo(int id_cliente);
    void siguiente_cliente();

private:
    mutex mtx_monitor;
    condition_variable esperar_cliente, esperar_barbero;
    bool barbero_disponible = true;
    int num_clientes_esperando = 0;
};

void barbero_dormilon::cortar_pelo(int id_cliente)
{
    unique_lock<mutex> lck(mtx_monitor);
    if (!barbero_disponible)
    {
        cout << "Cliente " << id_cliente << " esperando su turno." << endl;
        num_clientes_esperando++;
        esperar_cliente.wait(lck);
    }
    cout << "El barbero está cortando el pelo del cliente " << id_cliente << endl;
    barbero_disponible = false;
    cout << "El barbero ha terminado de cortar el pelo al cliente " << id_cliente << endl;
    barbero_disponible = true;
    esperar_barbero.notify_one();
}

void barbero_dormilon::siguiente_cliente()
{
    unique_lock<mutex> lck(mtx_monitor);
    if (num_clientes_esperando == 0)
    {
        cout << "El barbero está durmiendo." << endl;
        esperar_barbero.wait(lck);
    }
    num_clientes_esperando--;
    esperar_cliente.notify_one();
}

barbero_dormilon bd;

void cliente(int id)
{
    while (true)
    {
        bd.cortar_pelo(id);
    }
}

void barbero()
{
    while (true)
    {
        bd.siguiente_cliente();
    }
}

int main()
{
    thread hilo_barbero(barbero);

    thread hilos_clientes[5];
    for (int i = 0; i < 5; i++)
    {
        hilos_clientes[i] = thread(cliente, i + 1);
    }

    for (int i = 0; i < 5; i++)
    {
        hilos_clientes[i].join();
    }

    return 0;
}

---------------------------

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

using namespace std;

const int N = 5;

const int PENSANDO = 0;
const int COMIENDO = 1;

class ComidaFilosofos
{
public:
    void cogerPalillos(int i);
    void soltarPalillos(int i);

private:
    mutex mtxMonitor;
    condition_variable dormir[N];
    int estado[N] = {PENSANDO, PENSANDO, PENSANDO, PENSANDO, PENSANDO};
    void test(int k);
} cf;

void ComidaFilosofos::test(int k)
{
    if (estado[k] == PENSANDO && 
        estado[(k + N - 1) % N] != COMIENDO && 
        estado[(k + 1) % N] != COMIENDO)
    {
        estado[k] = COMIENDO;
        dormir[k].notify_all();
    }
}

void ComidaFilosofos::cogerPalillos(int i)
{
    unique_lock<mutex> lck(mtxMonitor);

    while (estado[(i + N - 1) % N] == COMIENDO || estado[(i + 1) % N] == COMIENDO)
    {
        dormir[i].wait(lck);
    }
    estado[i] = COMIENDO;
}

void ComidaFilosofos::soltarPalillos(int i)
{
    unique_lock<mutex> lck(mtxMonitor);
    estado[i] = PENSANDO;

    test((i + N - 1) % N);
    test((i + 1) % N);
}

void filosofo(int i)
{
    while (true)
    {
        cout << "El filosofo " << i + 1 << " esta pensando" << endl;
        this_thread::sleep_for(chrono::milliseconds(500));

        cf.cogerPalillos(i);
        cout << "El filosofo " << i + 1 << " esta comiendo" << endl;
        this_thread::sleep_for(chrono::milliseconds(500)); 

        cf.soltarPalillos(i);
        cout << "El filosofo " << i + 1 << " termino de comer" << endl;
    }
}

int main()
{
    thread filosofosThreads[N];

    for (int i = 0; i < N; i++)
    {
        filosofosThreads[i] = thread(filosofo, i);
    }

    for (int i = 0; i < N; i++)
    {
        filosofosThreads[i].join();
    }

    return 0;
}


-------------------------

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

using namespace std;

class le_monitor
{
public:
    void abrirLectura();
    void cerrarLectura();
    void abrirEscritura();
    void cerrarEscritura();

private:
    mutex mtxMonitor;
    condition_variable lector, escritor;
    void informa(string mens);
    int nl = 0;
    bool escribiendo = false;
};

void le_monitor::abrirLectura()
{
    unique_lock<mutex> lck(mtxMonitor);
    if (escribiendo || nl != 0)
    {
        lector.wait(lck);
    }
    nl++;
}

void le_monitor::cerrarLectura()
{
    unique_lock<mutex> lck(mtxMonitor);
    nl--;
    if (nl == 0)
    {
        escritor.notify_one();
    }
}

void le_monitor::abrirEscritura()
{
    unique_lock<mutex> lck(mtxMonitor);
    if (nl != 0 || escribiendo)
    {
        escritor.wait(lck);
    }
    escribiendo = true;
}

void le_monitor::cerrarEscritura()
{
    unique_lock<mutex> lck(mtxMonitor);
    escribiendo = false;
    if (nl != 0)
    {
        lector.notify_one();
    }
    else
    {
        escritor.notify_one();
    }
}

le_monitor le;

void lector()
{
    while (true)
    {
        le.abrirLectura();
        cout << "Leer" << endl;
        le.cerrarLectura();
    }
}

void escritor()
{
    while (true)
    {
        le.abrirEscritura();
        cout << "Escribir" << endl;
        le.cerrarEscritura();
    }
}

int main()
{
    thread l[3], e[3];

    for (int i = 0; i < 3; i++)
    {
        l[i] = thread(lector);
        e[i] = thread(escritor);
    }

    for (int i = 0; i < 3; i++)
    {
        l[i].join();
        e[i].join();
    }

    return 0;
}
