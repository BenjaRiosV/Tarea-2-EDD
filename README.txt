The Last of Us: Una Aventura de Texto

Esta tarea es un proyecto adaptado al universo post-apocalíptico de "The Last of Us", implementado en C++. El juego permite al jugador explorar un mundo desolado, enfrentarse a peligrosos enemigos, tomar decisiones cruciales en eventos interactivos y descubrir múltiples desenlaces para su historia.

----------------------
Integrantes del Equipo y Roles

Daniel Prieto (202473087-8) y Benjamín Ríos(202473081-9)
----------------------


Compilación:

Para compilar el proyecto, abre una terminal en el directorio donde se encuentran los archivos `main.cpp` y `data.map`. Luego, ejecuta el siguiente comando:

g++ main.cpp prototipo.cpp -o juego -Wall

Ejecución:

Una vez que el proyecto haya sido compilado exitosamente, puede ejecutar el juego desde la terminal utilizando el siguiente comando:

./juego


---------------

Estructura General del Archivo data.map

- El archivo `data.map` está constituido de la misma forma que la indicada en el documento en pdf de Tarea 2 - Estructuras de Datos (INF-134), el orden de las secciones es el siguiente:

1.  INICIO DE ARCHIVO
- Marca el punto de partida del archivo de datos.

2.  HABITACIONES
- Parte con la palabra "HABITACIONES", el número de habitaciones, luego los id de cada habitación, el nombre de la misma y, entre paréntesis, el tipo de habitación que representa, en la subsiguiente línea se encuentra la descripción de la habitación.

Ejemplo: 
    <id> <nombre_de_habitación> <(tipo_de_habitación)> 
    <descripción>

3.  ARCOS
- Parte con la palabra "ARCOS", seguido de esto, se lee la cantidad de arcos, y cada uno de los arcos que parten de una habitación de origen y llegan a una habitación final. 

Ejemplo:
<id habitacion ORIGEN>  ->  <id habitacion FIN>


4.  ENEMIGOS
- Parte con la palabra "ENEMIGOS", luego, lee la cantidad de enemigos, y describe cada uno de ellos en el siguiente formato:

Ejemplo:
<nombre de enemigo> | <"Vida"><valor> | <"Ataque"><valor> | <"Precision"><valor> | <"Probabilidad"><valor>

5.  EVENTOS
- Parte con la palabra "EVENTOS", luego, el número de eventos, un ampersand (&) que separa a  cada uno de los eventos, el nombre del evento, la probabilidad de que suceda el evento, la descripción del evento, las opciones y consecuencias de cada una. 

Ejemplo:
&
<Nombre_Evento>
Probabilidad <valor>
<Descripción_Evento>(La descripción del evento puede ocupar múltiples líneas. Se lee hasta encontrar una línea que empiece con "A:" o "B:".)
A: <Texto_Opcion_A>
<Consecuencia_Opcion_A_1>
B: <Texto_Opcion_B>
<Consecuencia_Opcion_B_1> 


6.  MEJORAS DE COMBATE
- Enumera las posibles mejoras estadísticas que el jugador puede obtener como recompensa después de ganar un combate.

Ejemplo:
+<Valor> <"Vida">
+<Valor> <"Precision">
+<Valor> <"Ataque">
+<Valor> <"Recuperación">

7.  FIN DE ARCHIVO
- Marca el final del archivo de datos.

--------------------------

Explicación de Requisito Especial

- Nuestro requisito especial consta en un evento especial que añade un nodo a la habitación de dicho evento, para cuando el jugador deba pasar a otra habitación, esta tendrá una habitación extra que no existiría en caso de haber ocurrido el evento especial. Esta habitación es añadida durante la ejecución y pertenece al data.map, sin embargo no tiene conexiones previas, por lo que solo se puede acceder luego del evento. Para que esta habitación sea accesible, en el evento especial llamado "Frasco extraño",  el jugador debe elegir la opción "Guardar frasco", que corresponde con la letra "A". Así, una vez acabado el evento, el jugador podrá acceder a una tercera habitación que si escogía la opción "B" no existiría. Este evento condiciona que las habitaciones de tipo EVENTO no puedan tener más de dos nodos hijos previamente, ya que el árbol ternario no está preparado para tener 4 nodos hijos en cada habitación.