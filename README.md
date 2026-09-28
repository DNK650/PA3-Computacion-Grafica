# PA3 – Renderizado de una escena 3D

## Proyecto
**Celda industrial robotizada 3D – OpenGL / C++**

Este proyecto continúa la escena desarrollada en el PA2 y la adapta al PA3 incorporando los elementos de renderizado solicitados: iluminación, materiales, sombreado, texturizado, visibilidad y un efecto complementario mediante animación.

## Objetivo
Renderizar una celda industrial 3D compuesta por varios objetos diferenciables, aplicando técnicas básicas de iluminación y sombreado para mejorar la percepción de volumen, materiales para distinguir superficies, una textura sobre el piso y control de visibilidad mediante Depth Buffer.

## Requerimientos implementados

| Requisito del PA3 | Implementación en el proyecto |
|---|---|
| Escena con al menos 6 objetos 3D | Robot articulado, base, cinta transportadora, caja, columnas, lámpara, piso y elementos de la celda |
| Cámara | Vista perspectiva y vistas frontal, lateral y superior |
| Depth Buffer | `GL_DEPTH_TEST` con `glDepthFunc(GL_LEQUAL)` |
| Mínimo 2 condiciones de iluminación | Iluminación ambiental global, luz puntual y luz direccional de relleno |
| Modelo de iluminación / sombreado | Iluminación fija de OpenGL con componentes ambiente, difusa y especular; `GL_SMOOTH` |
| Materiales diferenciados | Material mate, metálico y plástico con distinto nivel especular y brillo |
| Al menos una textura | Textura procedural tipo patrón industrial aplicada al piso |
| Efecto complementario | Animación de la caja sobre la cinta transportadora |

## Técnicas de renderizado utilizadas

### 1. Visibilidad
Se utiliza **Depth Buffer** para comparar la profundidad de los fragmentos y evitar que objetos posteriores se dibujen por encima de objetos más cercanos.

### 2. Iluminación
La escena utiliza tres aportes principales:
- **Luz ambiental global:** evita zonas completamente oscuras.
- **Luz puntual:** simula una lámpara dentro de la celda y posee atenuación con la distancia.
- **Luz direccional:** funciona como luz de relleno para apreciar mejor el volumen de los objetos.

### 3. Materiales
Se definieron tres comportamientos visuales simples:
- **Mate:** baja componente especular.
- **Metálico:** brillo especular alto.
- **Plástico:** brillo intermedio.

### 4. Textura
El piso utiliza una textura procedural generada en memoria. Se aplica con repetición para cubrir toda la superficie y puede activarse o desactivarse durante la ejecución.

### 5. Efecto complementario
La caja puede desplazarse automáticamente sobre la cinta transportadora. Esto permite conservar la interacción del PA2 y cumplir el requisito de incorporar un efecto adicional.

## Controles

### Pruebas principales del PA3
- `T` → activar/desactivar textura del piso.
- `K` → activar/desactivar luz puntual.
- `P` → cambiar la posición de la luz puntual.
- `F1` → vista perspectiva.
- `F2` → vista frontal.
- `F3` → vista lateral.
- `F4` → vista superior.
- `A` → iniciar/detener animación de la cinta.

### Controles adicionales
- `1 / 2` → girar base del robot.
- `3 / 4` → mover hombro.
- `5 / 6` → mover codo.
- `7 / 8` → mover muñeca.
- `9 / 0` → abrir/cerrar pinza.
- `J / L` → mover caja manualmente.
- `Z / X` → reducir/aumentar escala del robot.
- `Flechas` → orbitar cámara en perspectiva.
- `+ / -` → zoom.
- `W` → modo wireframe.
- `R` → reiniciar escena.
- `ESC` → salir.

## Pruebas de experimentación

Para evidenciar los cambios visuales solicitados se pueden realizar estas cuatro pruebas breves:

1. **Textura:** presionar `T` y comparar el piso con textura y sin textura.  
   **Cambio observado:** el patrón permite diferenciar mejor la superficie del piso y aporta detalle visual.

2. **Iluminación:** presionar `K` y comparar la escena con la luz puntual activa e inactiva.  
   **Cambio observado:** al activar la luz puntual aumentan las zonas iluminadas y los reflejos de los materiales cercanos.

3. **Posición de la luz:** presionar `P`.  
   **Cambio observado:** cambia la dirección de iluminación sobre los objetos y se modifican las zonas claras y oscuras de la escena.

4. **Cámara:** utilizar `F1`, `F2`, `F3` y `F4`.  
   **Cambio observado:** cada vista permite comprobar la posición, profundidad y oclusión de los objetos desde distintos ángulos.

## Compilación

### Requisitos
- Windows.
- CMake 3.20 o superior.
- Compilador C++ compatible con C++17.
- Conexión a Internet durante la primera configuración de CMake para descargar FreeGLUT 3.8.0 mediante `FetchContent`.

### Con CMake
```bash
cmake -S . -B build
cmake --build build
```

Después ejecutar el binario generado `PA3_Computacion_Grafica`.

## Estructura del proyecto

```text
PA3_Computacion_Grafica/
├── main.cpp
├── CMakeLists.txt
├── README_PA3.md
├── .gitignore
└── third_party/
    └── freeglut/
```

- `main.cpp`: escena, cámara, objetos, iluminación, materiales, textura, animación y controles.
- `CMakeLists.txt`: configuración de compilación y dependencia FreeGLUT.
- `README_PA3.md`: descripción técnica y guía de uso del proyecto.

## Integrantes

- Integrante 1: Meza Pastrana, Diego Armando
- Integrante 2: Castillo Espinoza, Welking Ronaldo
## Curso

**Computación Gráfica**

