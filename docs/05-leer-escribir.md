# Leer y escribir

Las dos funciones revisan la tabla antes de tocar `memory`, siempre en este orden: primero `valid` y después `access`.

## leer(page, offset)

1. Si `valid` es 0, hay fallo de página.
2. Si `access` es menor que 1, no hay permiso.
3. Si todo está bien, devuelve `memory[page][offset]`.

## escribir(page, offset, value)

1. Si `valid` es 0, hay fallo de página.
2. Si `access` no es 2, no hay permiso.
3. Si todo está bien, guarda `value` en `memory[page][offset]`.

Por ahora el fallo de página solo imprime un mensaje. Cuando estén los sockets, ahí se le va a pedir la página al dueño.
