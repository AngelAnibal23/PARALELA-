program prog;
monitor buffer;
export insertar,extraer;
const N=10;
var 
	tam,frente,final:integer;
	clleno,cvacio:condition;
	cola:array[0..N] of integer;
	
procedure insertar(elemento:integer);
begin
	if (tam=N) then
		delay(clleno);
	cola[final]:=elemento;
	final:=(final+1) mod N;
	tam:=tam+1;
	resume(cvacio);
end;

procedure extraer(var x:integer);
begin
	if tam=0 then
		delay(cvacio);
	x:=cola[frente];
	frente:=(frente+1) mod N;
	tam:=tam-1;
	resume(clleno);
end;

begin
	frente:=0;
	final:=0;
	tam:=0;
end;

process productor;
var y:integer;
begin
	repeat
		y:=random(10);
		buffer.insertar(y);
	forever
end;
process consumidor;
var
	a:integer;
begin
	repeat
		buffer.extraer(a);
		writeln(a);
	forever
end;

begin
	cobegin
		productor;
		consumidor;
	coend;
end.

------------

program prio;

monitor PrioLec;
export alectura, clectura, aescritura, cescritura;

var
  n1: integer;
  escribiendo: boolean;
  lector, escritor: condition;

  procedure alectura;
  begin
    if escribiendo then delay(lector);
    n1 := n1 + 1;
    resume(lector);
  end;

  procedure clectura;
  begin
    n1 := n1 - 1;
    if n1 = 0 then resume(escritor);
  end;

  procedure aescritura;
  begin
    if (n1 <> 0) or escribiendo then delay(escritor);
    escribiendo := true;
  end;

  procedure cescritura;
  begin
    escribiendo := false;
    if empty(lector) then resume(escritor)
    else resume(lector);
  end;

begin
  n1 := 0;
  escribiendo := false;
end;

process type lector;
var
  lec: integer;
begin
  repeat
    PrioLec.alectura;
    writeln('Leer el recurso');
    PrioLec.clectura;
  forever
end;

process type escritor;
begin
  repeat
    PrioLec.aescritura;
    writeln('Escribir en el recurso');
    PrioLec.cescritura;
  forever
end;

	var i : integer;
	LE: array[0..3] of lector;
	ES: array[0..3] of escritor;

begin
  	cobegin
		for i:=0 to 3 do
		LE[i]; ES[i];
	coend
end


..........----


program FilosofosComensales;

monitor comidaFilosofos;
	export cogerPalillos, soltarPalillos;
	const N = 5;
	var estado: array[0..4] of (pensando, comiendo, hambriento);
		dormir: array[0..4] of condition;
		i: integer;
	
	procedure test(k: integer);
	begin                                   *)
		if (estado[(k+N-1) mod N] <> comiendo) and
		   (estado[(k+1) mod N] <> comiendo) then
		begin 
			estado[k] := comiendo;
			resume(dormir[k]);
		end; 
	end;
	
	procedure cogerPalillos(i: integer);
	begin
		estado[i] := hambriento; 
		test(i);				
								 
		if estado[i] <> comiendo then delay(dormir[i])
	end;
	
	procedure soltarPalillos(i: integer);
	begin
		estado[i] := hambriento; 
								 
		test((i+4) mod N);
		test((i+1) mod N)
	end;
	
begin
	for i := 0 to N-1 do
		estado[i] := pensando;
end;

process type filosofo(i: integer);
begin
		writeln('El filosofo ', i+1, ' esta pensando');
		comidaFilosofos.cogerPalillos(i);
		writeln('El filosofo ', i+1, ' esta comiendo');
		comidaFilosofos.soltarPalillos(i);
		writeln('El filosofo ', i+1, ' termino de comer');
end;

var i: integer;
	F: array[0..4] of filosofo;
begin
	cobegin
		for i := 0 to 4 do
			F[i](i);
	coend;
end.

