# Cambios

## Por qué

`docs/00-definicion_proyecto.md` pide que en el demo 2 P2 mande un mensaje de invalidación a P1 antes de escribir. El código mandaba `PEDIR_ESCRITURA` y P1 invalidaba su copia al recibirlo, así que `INVALIDAR` no aparecía en ese demo.

## Qué cambió

### `src/dsm.c` (+16 líneas, nada borrado)

1. **`handle_page_fault`:** rama nueva antes de "no soy el dueño: se la pido a él". Si quiero escribir y ya tengo la página montada (`valid == 1`, o sea una copia de solo lectura):
   - mando `INVALIDAR`,
   - espero `ACK_INVALIDAR`,
   - pongo `owner = my_id` y `access = ACCESS_WRITE`.

   No se manda la página otra vez, porque ya la tengo.

2. **`handle_message`, caso `MSG_INVALIDAR`:** una línea nueva, `e->owner = 3 - my_id`. Quien recibe la invalidación cede la propiedad a quien la mandó.

### Documentación

- `docs/05-leer-escribir.md`: fila nueva en la tabla del fallo de página (escribir, no soy dueño pero tengo copia de solo lectura).
- `docs/08-protocolo.md`: filas de `INVALIDAR` y `ACK_INVALIDAR` actualizadas (ahora también cambia el dueño).
- `docs/09-demo.md`: demo 2 reescrito. Además, la secuencia que antes decía mostrar `INVALIDAR` ("P1 lee y P1 escribe") no lo mostraba; se reemplazó por una que sí.

## Cómo afecta la lógica anterior

- **Demo 1 (lectura):** igual.
- **Escribir una página que no tengo (`valid == 0`):** igual, sigue usando `PEDIR_ESCRITURA` y `ENVIAR_PAGINA`.
- **Dueño que escribe con una copia ajena:** igual en mensajes (`INVALIDAR` y `ACK_INVALIDAR`). La línea nueva en el receptor no cambia nada aquí, porque el dueño ya era quien mandaba.
- **Escribir con una copia de solo lectura sin ser dueño (demo 2):** esto es lo que cambió.
  - Antes: `PEDIR_ESCRITURA`, P1 invalidaba y mandaba la página completa.
  - Ahora: `INVALIDAR`, `ACK_INVALIDAR` y no se transfiere la página.
  - La tabla final es la misma: P1 `valid=0 access=0 owner=P2` y P2 `valid=1 access=2 owner=P2`.

## Lo que no cambió

Sockets (`net.c`), `main.c`, `leer`, `escribir`, `init_page_table`, la tabla de páginas, el formato de `message_t` y los tipos de mensaje.

## Pendiente

`diagramas/05-secuencia-demo2.svg` todavía muestra el flujo anterior con `PEDIR_ESCRITURA`.
