<?xml version="1.0" ?><!DOCTYPE TS><TS version="2.1" language="es" sourcelanguage="en">
<context>
    <name>AboutDialog</name>
    <message>
        <location filename="../src/gui/AboutDialog.ui" line="+14"/>
        <source>About Tagaini Jisho</source>
        <translation>Acerca de Tagaini Jisho</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Tagaini Jisho</source>
        <translation>Tagaini Jisho</translation>
    </message>
    <message>
        <location line="+29"/>
        <source>About</source>
        <translation>Acerca de</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Authors &amp;&amp; contributors</source>
        <translation>Autores y colaboradores</translation>
    </message>
</context>
<context>
    <name>BatchHandler</name>
    <message>
        <location filename="../src/gui/BatchHandler.cc" line="+26"/>
        <source>Marking entries...</source>
        <translation>Añadiendo entradas...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Abort</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Please wait...</source>
        <translation>Espere por favor...</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Cannot start transaction</source>
        <translation>No se puede iniciar la operación</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Error while trying to start database transaction.</source>
        <translation>Error al intentar iniciar la operación con la base de datos.</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Cannot commit transaction</source>
        <translation>No se puede realizar la operación</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Error while trying to commit database transaction.</source>
        <translation>Error al intentar realizar la operación con la base de datos.</translation>
    </message>
</context>
<context>
    <name>DataPreferences</name>
    <message>
        <location filename="../src/gui/DataPreferences.ui" line="+17"/>
        <source>Erase user data</source>
        <translation>Eliminar datos del usuario</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Pushing this button will completely erase all your user data. This includes study list, tags, notes, scores, basically everything you did. After pushing this button, Tagaini Jisho will exit. Upon restart, you will be back to a fresh, blank database.</source>
        <translation>Pulsar este botón eliminará todos los datos del usuario. Esto incluye listas de estudio, etiquetas, notas, puntuaciones, ... Básicamente, todo lo que el usuario ha hecho. Tras pulsar este botón, Tagaini Jisho se cerrará. Al reiniciarse, volverá a tener una base de datos vacía.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Reset user data</source>
        <translation>Eliminar datos del usuario</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>User database file:</source>
        <translation>Archivo de la base de datos del usuario:</translation>
    </message>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="+497"/>
        <source>User data</source>
        <translation>Datos de usuario</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Really erase user data?</source>
        <translation>¿Está seguro de querer eliminar los datos del usuario?</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>This will erase all your user data. Are you sure you want to do this?</source>
        <translation>Esto eliminará todos sus datos de usuario. ¿Está seguro de querer hacer esto?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>REALLY erase it?</source>
        <translation>¿REALMENTE desea eliminarlos?</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Are you absolutely, positively, definitely sure?</source>
        <translation>¿Está total y absolutamente seguro?</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>No user database file</source>
        <translation>La base de datos del usuario no existe</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The user database file has already been erased before.</source>
        <translation>La base de datos del usuario ha sido eliminada previamente.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Cannot erase user database</source>
        <translation>No es posible eliminar la base de datos del usuario</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unable to erase user database. Please see what is wrong and try to do it manually.</source>
        <translation>No es posible eliminar la base de datos del usuario. Por favor, fíjese en qué es incorrecto y trate de hacerlo manualmente.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>User data erased</source>
        <translation>Datos del usuario eliminados</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>User data has been erased. Tagaini Jisho will now exit. You will be back to a blank database when it is restarted.</source>
        <translation>Los datos del usuario han sido eliminados. Tagaini Jisho se cerrará. Se usará una base de datos vacía tras reiniciar.</translation>
    </message>
</context>
<context>
    <name>Database</name>
    <message>
        <location filename="../src/core/Database.cc" line="+316"/>
        <source>Error while upgrading user database: %1</source>
        <translation>Error al actualizar la base de datos del usuario: %1</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Wrong user database version: expected %1, got %2.</source>
        <translation>La versión de la base de datos del usuario es incorrecta: se esperaba %1, pero se obtuvo %2.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Cannot create user database: %1</source>
        <translation>No se puede crear la base de datos del usuario: %1</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Cannot open database: %1</source>
        <translation>No se puede abrir la base de datos: %1</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Temporary database fallback failed. The program will now exit.</source>
        <translation>La base de datos temporal de reserva ha fallado. El programa se cerrará ahora.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Tagaini is working on a temporary database. This allows the program to work, but user data is unavailable and any change will be lost upon program exit. If you corrupted your database file, please recreate it from the preferences.</source>
        <translation>Tagaini está trabajando con una base de datos temporal. Esto permite que el programa funcione, pero los datos del usuario no estarán disponibles y cualquier cambio se perderá al salir del programa. Si ha dañado su archivo de base de datos, por favor, intente recrearlo a partir de las preferencias.</translation>
    </message>
</context>
<context>
    <name>DetailedView</name>
    <message>
        <location filename="../src/gui/DetailedView.cc" line="+101"/>
        <source>Previous entry</source>
        <translation>Entrada anterior</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Next entry</source>
        <translation>Siguiente entrada</translation>
    </message>
</context>
<context>
    <name>DetailedViewPreferences</name>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="-108"/>
        <source>Detailed view</source>
        <translation>Vista detallada</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Default text</source>
        <translation>Texto por defecto</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kana header</source>
        <translation>Kana de cabecera</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kanji header</source>
        <translation>Kanji de cabecera</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kanji</source>
        <translation>Kanji</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Kana</source>
        <translation>Kana</translation>
    </message>
    <message>
        <location filename="../src/gui/DetailedViewPreferences.ui" line="+17"/>
        <source>General</source>
        <translation>General</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Smooth scrolling</source>
        <translation>Desplazamiento suave</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Show JLPT level in short descriptions</source>
        <translation>Mostrar el nivel JLPT en la descripción corta</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Fonts</source>
        <translation>Fuentes</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Preview</source>
        <translation>Previsualización</translation>
    </message>
</context>
<context>
    <name>EditEntryNotesDialog</name>
    <message>
        <location filename="../src/gui/EditEntryNotesDialog.cc" line="+86"/>
        <source>Notes for %1</source>
        <translation>Notas para %1</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Existing notes:</source>
        <translation>Notas existentes:</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>&amp;New note</source>
        <translation>&amp;Nueva nota</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>&amp;Delete note</source>
        <translation>&amp;Eliminar nota</translation>
    </message>
</context>
<context>
    <name>EntriesPrinter</name>
    <message>
        <location filename="../src/gui/EntriesPrinter.cc" line="+66"/>
        <source>Preparing print job...</source>
        <translation>Preparando la impresión...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Abort</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Printing...</source>
        <translation>Imprimiendo...</translation>
    </message>
    <message>
        <location line="+89"/>
        <source>Print preview</source>
        <translation>Previsualización de la impresión</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Booklet print preview</source>
        <translation>Previsualización de la impresión del cuaderno</translation>
    </message>
</context>
<context>
    <name>EntriesViewHelper</name>
    <message>
        <location filename="../src/gui/EntriesViewHelper.cc" line="+42"/>
        <source>&amp;Print...</source>
        <translation>Im&amp;primir...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Print p&amp;review...</source>
        <translation>&amp;Vista preliminar...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Print &amp;booklet...</source>
        <translation>Imprimir &amp;cuaderno...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Export as &amp;HTML...</source>
        <translation>Exportar como &amp;HTML...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Booklet pre&amp;view...</source>
        <translation>Pre&amp;visualización del cuaderno...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Export as &amp;TSV...</source>
        <translation>Exportar como &amp;TSV...</translation>
    </message>
    <message>
        <location line="+57"/>
        <source>Abort</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Selecting entries...</source>
        <translation>Seleccionando entradas...</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Please wait...</source>
        <translation>Espere por favor...</translation>
    </message>
    <message>
        <location line="+259"/>
        <source>Print preview</source>
        <translation>Previsualización de la impresión</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Booklet print</source>
        <translation>Imprimir cuaderno</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Booklet print preview</source>
        <translation>Previsualización de la impresión del cuaderno</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Export to tab-separated file...</source>
        <translation>Exportar como texto separado por tabulaciones...</translation>
    </message>
    <message>
        <location line="+4"/>
        <location line="+47"/>
        <source>Cannot write file</source>
        <translation>No es posible escribir el archivo</translation>
    </message>
    <message>
        <location line="-47"/>
        <source>Unable to write file %1.</source>
        <translation>No es posible escribir el archivo %1.</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Error writing file</source>
        <translation>Error al escribir el archivo</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Error while writing file %1.</source>
        <translation>Error mientras se escribía el archivo %1.</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Export to HTML flashcard file...</source>
        <translation>Exportar a un archivo de tarjetas de vocabulario HTML...</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Unable to write file %1!</source>
        <translation>¡No es posible escribir el archivo %1!</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Cannot open template file</source>
        <translation>No es posible abrir el archivo de plantilla</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unable to open template file!</source>
        <translation>¡No es posible abrir el archivo de plantilla!</translation>
    </message>
    <message>
        <location line="-94"/>
        <location filename="../src/gui/EntriesViewHelper.h" line="+90"/>
        <source>Print</source>
        <translation>Imprimir</translation>
    </message>
</context>
<context>
    <name>EntryDelegatePreferences</name>
    <message>
        <location filename="../src/gui/EntryDelegatePreferences.ui" line="+19"/>
        <source>Style:</source>
        <translation>Estilo:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>One &amp;line</source>
        <translation>Una &amp;línea</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>&amp;Two lines</source>
        <translation>Dos líneas (T)</translation>
    </message>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="-196"/>
        <source>Main writing</source>
        <translation>Escritura principal</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Readings and alternate writings</source>
        <translation>Lecturas y otras escrituras</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Definitions</source>
        <translation>Definiciones</translation>
    </message>
</context>
<context>
    <name>EntryDragButton</name>
    <message>
        <location filename="../src/gui/ToolBarDetailedView.cc" line="+29"/>
        <source>Drag the currently displayed entry</source>
        <translation>Arrastrar la entrada que se está mostrando</translation>
    </message>
</context>
<context>
    <name>EntryFormatter</name>
    <message>
        <location filename="../src/gui/EntryFormatter.cc" line="+57"/>
        <source>Cannot find detailed view HTML file!</source>
        <translation>¡No se pudo encontrar el archivo HTML de la vista detallada!</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Cannot find detailed view CSS file!</source>
        <translation>¡No se pudo encontrar el archivo CSS de la vista detallada!</translation>
    </message>
    <message>
        <location line="+181"/>
        <source>Root list</source>
        <translation>lista raíz</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>Studied since %1.</source>
        <translation>Estudiado desde el %1.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Score: %1.</source>
        <translation>Puntuación: %1.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Last trained on %1.</source>
        <translation>Practicado por última vez el %1.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Last mistaken on %1.</source>
        <translation>Fallado por última vez el %1.</translation>
    </message>
</context>
<context>
    <name>EntryListView</name>
    <message>
        <location filename="../src/gui/EntryListView.cc" line="+36"/>
        <source>Delete</source>
        <translation>Eliminar</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>New list...</source>
        <translation>Nueva lista...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Go up</source>
        <translation>Subir</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Rename list...</source>
        <translation>Renombrar lista...</translation>
    </message>
    <message>
        <location line="+93"/>
        <source>Unable to create list</source>
        <translation>Imposible crear la lista</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>A database error occured while trying to add the list.</source>
        <translation>Ocurrió un error con la base de datos al intentar añadir la lista.</translation>
    </message>
    <message>
        <location line="+39"/>
        <source>Confirm deletion</source>
        <translation>Confirmar eliminación</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>This will delete the selected lists items and lists, including all their children. Continue?</source>
        <translation>Esto eliminará las listas e ítemes seleccionados, incluyendo todo su contenido. ¿Continuar?</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Removal failed</source>
        <translation>Eliminación fallida</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>A database error has occured while trying to remove the selected items:

%1

 Some of them may be remaining.</source>
        <translation>Ha ocurrido un error de la base de datos al intentar eliminar los ítemes seleccionados:

%1

Algunos pueden seguir existiendo.</translation>
    </message>
</context>
<context>
    <name>EntryListWidget</name>
    <message>
        <location filename="../src/gui/EntryListWidget.h" line="+33"/>
        <source>Lists</source>
        <translation>Listas</translation>
    </message>
</context>
<context>
    <name>EntryMenu</name>
    <message>
        <location filename="../src/gui/EntryMenu.cc" line="+28"/>
        <source>Add to &amp;study list</source>
        <translation>Agregar a la lista de e&amp;studio</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Remove from &amp;study list</source>
        <translation>Eliminar de la lista de e&amp;studio</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Already &amp;known</source>
        <translation>Ya &amp;conocido</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>&amp;Reset score</source>
        <translation>&amp;Reiniciar la puntuación</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Set &amp;tags...</source>
        <translation>Editar e&amp;tiquetas...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>&amp;Add tags...</source>
        <translation>&amp;Añadir etiquetas...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Edit &amp;notes...</source>
        <translation>Editar &amp;notas...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Recently added tags...</source>
        <translation>Etiquetas recientes...</translation>
    </message>
    <message>
        <location line="+87"/>
        <location line="+6"/>
        <source>Copy &quot;%1&quot; to clipboard</source>
        <translation>Copiar &quot;%1% al portapapeles</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Add &quot;%1&quot;</source>
        <translation>Añadir &quot;%1&quot;</translation>
    </message>
</context>
<context>
    <name>EntryNotesModel</name>
    <message>
        <location filename="../src/gui/EditEntryNotesDialog.cc" line="-65"/>
        <source>Notes</source>
        <translation>Notas</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>&lt;New note&gt;</source>
        <translation>&lt;Nueva nota&gt;</translation>
    </message>
</context>
<context>
    <name>EntryTypeFilterWidget</name>
    <message>
        <location filename="../src/gui/EntryTypeFilterWidget.cc" line="+33"/>
        <source>All</source>
        <translation>Todo</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Vocabulary</source>
        <translation>Vocabulario</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Characters</source>
        <translation>Caracteres</translation>
    </message>
    <message>
        <location filename="../src/gui/EntryTypeFilterWidget.h" line="+43"/>
        <source>Entry type filter</source>
        <translation>Filtro de tipo de búsqueda</translation>
    </message>
</context>
<context>
    <name>FindHomographsJob</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictEntryFormatter.cc" line="+625"/>
        <source>Homographs</source>
        <translation>Homografías</translation>
    </message>
</context>
<context>
    <name>FindHomophonesJob</name>
    <message>
        <location line="-23"/>
        <source>Homophones</source>
        <translation>Homofonías</translation>
    </message>
</context>
<context>
    <name>FindVerbBuddyJob</name>
    <message>
        <location line="-15"/>
        <source>Transitive buddy</source>
        <translation>Contraparte transitiva</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Intransitive buddy</source>
        <translation>Contraparte intransitiva</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Buddy</source>
        <translation>Contraparte</translation>
    </message>
</context>
<context>
    <name>GeneralPreferences</name>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="-166"/>
        <source>English</source>
        <translation>Inglés</translation>
    </message>
    <message>
        <location line="-1"/>
        <source>German</source>
        <translation>Alemán</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>French</source>
        <translation>Francés</translation>
    </message>
    <message>
        <location line="-3"/>
        <source>Spanish</source>
        <translation>Español</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Norvegian Bokmal</source>
        <translation>Noruego bokmål</translation>
    </message>
    <message>
        <location line="-11"/>
        <source>Czech</source>
        <translation>Checo</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Dutch</source>
        <translation>Neerlandés</translation>
    </message>
    <message>
        <location line="-3"/>
        <source>Italian</source>
        <translation>Italiano</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Portuguese (Brazil)</source>
        <translation>Portugués (Brasil)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Russian</source>
        <translation>Ruso</translation>
    </message>
    <message>
        <location line="-5"/>
        <source>Japanese</source>
        <translation>Japonés</translation>
    </message>
    <message>
        <location line="-11"/>
        <source>Arabic</source>
        <translation>Árabe</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Persian (Iran)</source>
        <translation>Persa (Irán)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Finnish (Finland)</source>
        <translation>Finlandés (Finlandia)</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Hungarian</source>
        <translation>Húngaro</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Indonesian</source>
        <translation>Indonesio</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Polish</source>
        <translation>Polaco</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Swedish</source>
        <translation>Sueco</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Thai</source>
        <translation>Tailandés</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Turkish</source>
        <translation>Turco</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ukrainian</source>
        <translation>Ucraniano</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Vietnamese</source>
        <translation>Vitnamita</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Chinese</source>
        <translation>Chino</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>General</source>
        <translation>General</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Application-wide default font</source>
        <translation>Fuente por defecto del programa</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Monday</source>
        <translation>Lunes</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sunday</source>
        <translation>Domingo</translation>
    </message>
    <message>
        <location line="+48"/>
        <source>Next check: %1</source>
        <translation>Siguiente comprobación: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/GeneralPreferences.ui" line="+17"/>
        <source>General settings</source>
        <translation>Opciones generales</translation>
    </message>
    <message>
        <location line="+35"/>
        <source>System default</source>
        <translation>Por defecto del sistema</translation>
    </message>
    <message>
        <location line="-27"/>
        <source>Preferred dictionary language</source>
        <translation>Idioma preferido para el diccionario</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Preferred &amp;GUI language</source>
        <translation>Idioma preferido para la Interfaz visual (G)</translation>
    </message>
    <message>
        <location line="+34"/>
        <source>The dictionaries will use this language whenever possible. English will be used as a fallback.</source>
        <translation>El diccionario usará este idioma siempre que sea posible. Se usará el Inglés como alternativa.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Same as GUI</source>
        <translation>Igual que el programa</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>You need to restart Tagaini Jisho for these changes to take effect.</source>
        <translation>Se debe reiniciar Tagaini Jisho para que estos cambios surtan efecto.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Dates</source>
        <translation>Fechas</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Updates</source>
        <translation>Actualizaciones</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Check for updates</source>
        <translation>Comprobar actualizaciones</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>every day</source>
        <translation>cada día</translation>
    </message>
    <message>
        <location line="+3"/>
        <source> days</source>
        <translation> días</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>every </source>
        <translation>cada </translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Also check for development versions</source>
        <translation>Buscar también versiones en desarrollo</translation>
    </message>
    <message>
        <location line="-113"/>
        <source>The GUI will use this language whenever possible. English will be used as a fallback.</source>
        <translation>El programa usará este idioma siempre que sea posible. Se usará el Inglés como alternativa.</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>If checked, English meanings will be shown even if a translation in the preferred language is available.</source>
        <translation>Si es marcado, el significado en Inglés será mostrado aún cuando hay una traducción en el lenguaje preferido.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Always show English meanings</source>
        <translation>Siempre mostrar significado en Inglés</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Wee&amp;ks start on</source>
        <translation>Semana comienza en (K)</translation>
    </message>
    <message>
        <location line="+59"/>
        <source>Next check:</source>
        <translation>Siguiente comprobación:</translation>
    </message>
</context>
<context>
    <name>JLPTFilterWidget</name>
    <message>
        <location filename="../src/gui/JLPTFilterWidget.cc" line="+28"/>
        <source>N5</source>
        <translation>N5</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>N4</source>
        <translation>N4</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>N3</source>
        <translation>N3</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>N2</source>
        <translation>N2</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>N1</source>
        <translation>N1</translation>
    </message>
    <message>
        <location line="+78"/>
        <source>JLPT:</source>
        <translation>JLPT: </translation>
    </message>
    <message>
        <location line="+1"/>
        <source>JLPT</source>
        <translation>JLPT</translation>
    </message>
</context>
<context>
    <name>JMdictEntry</name>
    <message>
        <location filename="../src/core/jmdict/JMdictEntry.cc" line="+102"/>
        <source>DELETED ENTRY</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+22"/>
        <source>Click for details</source>
        <translation>Click para más detalles</translation>
    </message>
</context>
<context>
    <name>JMdictEntryFormatter</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictEntryFormatter.cc" line="-369"/>
        <source>(JLPT N%1)</source>
        <translation>(JLPT N%1)</translation>
    </message>
    <message>
        <location line="+38"/>
        <source>Alternate readings: </source>
        <translation>Lecturas alternativas: </translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Alternate writings:</source>
        <translation>Escrituras alternativas:</translation>
    </message>
    <message>
        <location line="+64"/>
        <source>&lt;p&gt;This entry has been removed from the JMdict and cannot be displayed.&lt;/p&gt;</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&lt;p&gt;Please find a replacement and remove this entry from your study list, tags, and notes. To help you find a replacement, the legacy content of this entry is visible &lt;a href=&quot;%1&quot;&gt;here&lt;/a&gt;.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+62"/>
        <source>JLPT</source>
        <translation>JLPT</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Kanji</source>
        <translation>Kanji</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Example sentences</source>
        <translation>Frases de ejemplo</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>JMdict ID</source>
        <translation>ID JMdict</translation>
    </message>
</context>
<context>
    <name>JMdictFilterWidget</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictFilterWidget.cc" line="+42"/>
        <source>Using studied kanji only</source>
        <translation>Usando sólo los kanjis estudiados</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>With kanji:</source>
        <translation>Contiene los kanjis:</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>With components:</source>
        <translation>Contiene los componentes:</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Include kana-only words</source>
        <translation>Incluir palabras con sólo kana</translation>
    </message>
    <message>
        <location line="+13"/>
        <location line="+117"/>
        <source>Part of speech</source>
        <translation>Categoría gramatical</translation>
    </message>
    <message>
        <location line="-111"/>
        <source>Dialect</source>
        <translation>Dialecto</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Field</source>
        <translation>Campo</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Misc</source>
        <translation>Otros</translation>
    </message>
    <message>
        <location line="+53"/>
        <source> with </source>
        <translation> con </translation>
    </message>
    <message>
        <location line="+9"/>
        <source> with component </source>
        <translation> con los componentes </translation>
    </message>
    <message>
        <location line="+9"/>
        <source> with studied kanji</source>
        <translation> sólo kanjis estudiados </translation>
    </message>
    <message>
        <location line="+4"/>
        <source> using kana only</source>
        <translation> sólo usando kanas</translation>
    </message>
    <message>
        <location line="-5"/>
        <source>, studied kanji only</source>
        <translation>, sólo kanjis estudiados</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>, including kana words</source>
        <translation>, incluyendo palabras con kanas</translation>
    </message>
    <message>
        <location line="+9"/>
        <location line="+1"/>
        <source>Vocabulary</source>
        <translation>Vocabulario</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Pos:</source>
        <translation>Cat:</translation>
    </message>
</context>
<context>
    <name>JMdictGUIPlugin</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictGUIPlugin.cc" line="+61"/>
        <source>Vocabulary flashcards</source>
        <translation>Tarjetas de vocabulario</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;japanese, whole study list</source>
        <translation>Del &amp;japonés, toda la lista de estudio</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;japanese, current set</source>
        <translation>Del &amp;japonés, la selección actual</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>From &amp;translation, whole study list</source>
        <translation>De la &amp;traducción, toda la lista de estudio</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;translation, current set</source>
        <translation>De la &amp;traducción, la selección actual</translation>
    </message>
    <message>
        <location line="+104"/>
        <location line="+19"/>
        <source>Nothing to train</source>
        <translation>Nada para entrenar</translation>
    </message>
    <message>
        <location line="-19"/>
        <location line="+19"/>
        <source>There are no vocabulary entries in this set to train on.</source>
        <translation>No hay entradas de vocabulario en la selección actual para entrenar.</translation>
    </message>
</context>
<context>
    <name>JMdictLongDescs</name>
    <message>
        <location filename="../3rdparty/JMdictDescs.cc" line="+67"/>
        <source>military</source>
        <translation>militar</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>mathematics</source>
        <translation>matemáticas</translation>
    </message>
    <message>
        <location line="-56"/>
        <source>Nagano-ben</source>
        <translation>Nagano-ben</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Kyoto-ben</source>
        <translation>Kyoto-ben</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Tsugaru-ben</source>
        <translation>Tsugaru-ben</translation>
    </message>
    <message>
        <location line="-1"/>
        <source>Tosa-ben</source>
        <translation>Tosa-ben</translation>
    </message>
    <message>
        <location line="-5"/>
        <source>Kyuushuu-ben</source>
        <translation>Kyuushuu-ben</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Ryuukyuu-ben</source>
        <translation>Ryuukyuu-ben</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>Kansai-ben</source>
        <translation>Kansai-ben</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Osaka-ben</source>
        <translation>Osaka-ben</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Touhoku-ben</source>
        <translation>Touhoku-ben</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>Kantou-ben</source>
        <translation>Kantou-ben</translation>
    </message>
    <message>
        <location line="+126"/>
        <source>manga slang</source>
        <translation>jerga del manga</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>polite (teineigo) language</source>
        <translation>lenguaje cortés (teineigo)</translation>
    </message>
    <message>
        <location line="+105"/>
        <source>su verb - precursor to the modern suru</source>
        <translation>Verbo su - precursor del actual suru</translation>
    </message>
    <message>
        <location line="-245"/>
        <source>Hokkaido-ben</source>
        <translation>Hokkaido-ben</translation>
    </message>
    <message>
        <location line="+154"/>
        <source>vulgar expression or word</source>
        <translation>palabra o expresión vulgar</translation>
    </message>
    <message>
        <location line="-29"/>
        <source>idiomatic expression</source>
        <translation>modismo</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>honorific or respectful (sonkeigo) language</source>
        <translation>Lenguaje honorífico o respetuoso (sonkeigo)</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>female term or language</source>
        <translation>Lenguaje o término femenino</translation>
    </message>
    <message>
        <location line="-118"/>
        <source>Brazilian</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+12"/>
        <source>agriculture</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>anatomy</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>archeology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>architecture</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>art, aesthetics</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>astronomy</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>audiovisual</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>aviation</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>baseball</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>biochemistry</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>biology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>botany</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Buddhism</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>business</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>card games</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>chemistry</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Christianity</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>clothing</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>computing</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>crystallography</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>dentistry</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>ecology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>economics</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>electricity, elec. eng.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>electronics</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>embryology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>engineering</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>entomology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>film</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>finance</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>fishing</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>food, cooking</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>gardening, horticulture</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>genetics</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>geography</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>geology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>geometry</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>go (game)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>golf</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>grammar</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Greek mythology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>hanafuda</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>horse racing</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>kabuki</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>law</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>linguistics</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>logic</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>martial arts</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>mahjong</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>manga</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>mechanical engineering</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>medicine</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>meteorology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>mining</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>music</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>noh</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>ornithology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>paleontology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>pathology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+182"/>
        <source>search-only kana form</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="-180"/>
        <source>philosophy</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>photography</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>physics</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>physiology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>printing</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>psychiatry</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>psychology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>railway</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>Shinto</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>shogi</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>sports</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>statistics</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>sumo</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>telecommunications</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>trademark</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>video games</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>zoology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>ateji (phonetic) reading</source>
        <translation>Ateji (lectura fonética)</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>word containing out-dated kanji or kanji usage</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>rarely-used kanji form</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+4"/>
        <source>character</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>children&apos;s language</source>
        <translation>Lenguaje infantil</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>creature</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>deity</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>document</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>event</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+3"/>
        <source>fiction</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>formal or literary term</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>group</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+6"/>
        <source>legend</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+3"/>
        <source>mythology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>object</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+4"/>
        <source>other</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+9"/>
        <source>religion</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>service</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+14"/>
        <source>&apos;kari&apos; adjective (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&apos;ku&apos; adjective (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+3"/>
        <source>nouns which may take the genitive case particle &apos;no&apos;</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>&apos;shiku&apos; adjective (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&apos;taru&apos; adjective</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>adverb taking the &apos;to&apos; particle</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+5"/>
        <source>copula</source>
        <translation>cópula</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>expressions (phrases, clauses, etc.)</source>
        <translation>expresiones (frases, cláusulas, etc.)</translation>
    </message>
    <message>
        <location line="+73"/>
        <source>gikun (meaning as reading) or jukujikun (special kanji reading)</source>
        <translation>gikun (significado similar a la lectura) o jukujikun (lectura especial)</translation>
    </message>
    <message>
        <location line="-152"/>
        <source>word containing irregular kanji usage</source>
        <translation>Palabra con uso irregular de kanji</translation>
    </message>
    <message>
        <location line="-1"/>
        <location line="+154"/>
        <source>word containing irregular kana usage</source>
        <translation>Palabra con uso irregular de kana</translation>
    </message>
    <message>
        <location line="-152"/>
        <source>irregular okurigana usage</source>
        <translation>Uso irregular de Okurigana</translation>
    </message>
    <message>
        <location line="+153"/>
        <source>out-dated or obsolete kana usage</source>
        <translation>Uso de kana anticuado u obsoleto</translation>
    </message>
    <message>
        <location line="-118"/>
        <source>onomatopoeic or mimetic word</source>
        <translation>Onomatopeya o mimético</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>poetical term</source>
        <translation>poética</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>proverb</source>
        <translation>proverbio</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>quotation</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+3"/>
        <source>sensitive</source>
        <translation>sensible (palabras susceptibles de crítica)</translation>
    </message>
    <message>
        <location line="+44"/>
        <source>unclassified</source>
        <translation>no clasificado</translation>
    </message>
    <message>
        <location line="-33"/>
        <source>yojijukugo</source>
        <translation>yojijukugo</translation>
    </message>
    <message>
        <location line="+36"/>
        <source>Ichidan verb - kureru special class</source>
        <translation>verbo ichidan - clase especial kureru</translation>
    </message>
    <message>
        <location line="+53"/>
        <source>suru verb - included</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="-82"/>
        <source>archaic/formal form of na-adjective</source>
        <translation>adjetivo &apos;na&apos; (forma arcaica/formal)</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>proper noun</source>
        <translation>nombre propio</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>verb unspecified</source>
        <translation>verbo sin especificar</translation>
    </message>
    <message>
        <location line="-60"/>
        <source>Internet slang</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="-21"/>
        <source>dated term</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+12"/>
        <source>historical term</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+29"/>
        <source>family or surname</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="-13"/>
        <source>place name</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+15"/>
        <source>unclassified name</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="-45"/>
        <source>company name</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+33"/>
        <source>product name</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+14"/>
        <source>work of art, literature, music, etc. name</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="-18"/>
        <source>full name of a particular person</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="-17"/>
        <source>given name or forename, gender not specified</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+30"/>
        <source>railway station</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="-15"/>
        <source>organization name</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+25"/>
        <source>adjective (keiyoushi) - yoi/ii class</source>
        <translation>adjetivo (keiyoushi) - clase yoi/ii</translation>
    </message>
    <message>
        <location line="-34"/>
        <source>jocular, humorous term</source>
        <translation>jocoso o humorístico</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>humble (kenjougo) language</source>
        <translation>Lenguaje humilde (kenjougo)</translation>
    </message>
    <message>
        <location line="-21"/>
        <source>abbreviation</source>
        <translation>abreviación</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>familiar language</source>
        <translation>Lenguaje familiar</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>rude or X-rated term (not displayed in educational software)</source>
        <translation>Término grosero o sexual (no usado en programas educativos)</translation>
    </message>
    <message>
        <location line="-44"/>
        <source>derogatory</source>
        <translation>despectivo</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>word usually written using kana alone</source>
        <translation>palabra escrita normalmente usando sólo kanas</translation>
    </message>
    <message>
        <location line="-3"/>
        <source>slang</source>
        <translation>jerga</translation>
    </message>
    <message>
        <location line="-20"/>
        <source>male term or language</source>
        <translation>Lenguaje o término masculino</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>obsolete term</source>
        <translation>Término obsoleto</translation>
    </message>
    <message>
        <location line="+51"/>
        <source>noun (temporal) (jisoumeishi)</source>
        <translation>Sustantivo (temporal) (jisoumeishi)</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>prefix</source>
        <translation>prefijo</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>interjection (kandoushi)</source>
        <translation>Interjección (kandoushi)</translation>
    </message>
    <message>
        <location line="+61"/>
        <source>Godan verb - Uru old class verb (old form of Eru)</source>
        <translation>Verbo Godan - antiguo verbo Uru (forma antigua de Eru)</translation>
    </message>
    <message>
        <location line="-44"/>
        <source>Nidan verb (upper class) with &apos;bu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="-123"/>
        <source>pharmacology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+5"/>
        <source>politics</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+3"/>
        <source>psychoanalysis</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+3"/>
        <source>Roman mythology</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+3"/>
        <source>skiing</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+3"/>
        <source>stock market</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+4"/>
        <source>television</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+9"/>
        <source>search-only kanji form</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>archaic</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+3"/>
        <source>colloquial</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+7"/>
        <source>euphemistic</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+30"/>
        <source>rare term</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+4"/>
        <source>ship name</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+48"/>
        <source>Nidan verb (lower class) with &apos;bu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (upper class) with &apos;dzu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (lower class) with &apos;dzu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (upper class) with &apos;gu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (lower class) with &apos;gu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (upper class) with &apos;hu/fu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (lower class) with &apos;hu/fu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (upper class) with &apos;ku&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (lower class) with &apos;ku&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (upper class) with &apos;mu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (lower class) with &apos;mu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (lower class) with &apos;nu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (upper class) with &apos;ru&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (lower class) with &apos;ru&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (lower class) with &apos;su&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (upper class) with &apos;tsu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (lower class) with &apos;tsu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (lower class) with &apos;u&apos; ending and &apos;we&apos; conjugation (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (upper class) with &apos;yu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (lower class) with &apos;yu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Nidan verb (lower class) with &apos;zu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Yodan verb with &apos;bu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Yodan verb with &apos;gu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Yodan verb with &apos;hu/fu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Yodan verb with &apos;ku&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Yodan verb with &apos;mu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Yodan verb with &apos;nu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Yodan verb with &apos;ru&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Yodan verb with &apos;su&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Yodan verb with &apos;tsu&apos; ending (archaic)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>Godan verb with &apos;bu&apos; ending</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Godan verb with &apos;gu&apos; ending</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Godan verb with &apos;ku&apos; ending</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>Godan verb with &apos;mu&apos; ending</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Godan verb with &apos;nu&apos; ending</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Godan verb with &apos;ru&apos; ending</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Godan verb with &apos;ru&apos; ending (irregular verb)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Godan verb with &apos;su&apos; ending</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Godan verb with &apos;tsu&apos; ending</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Godan verb with &apos;u&apos; ending</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Godan verb with &apos;u&apos; ending (special class)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+2"/>
        <source>intransitive verb</source>
        <translation>Verbo intransitivo</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Kuru verb - special class</source>
        <translation>Verbo kuru - clase especial</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>irregular nu verb</source>
        <translation>Verbo nu irregular</translation>
    </message>
    <message>
        <location line="-54"/>
        <source>particle</source>
        <translation>partícula</translation>
    </message>
    <message>
        <location line="+55"/>
        <source>irregular ru verb, plain form ends with -ri</source>
        <translation>Verbo ru irregular, forma simple acabada en &apos;-ri&apos;</translation>
    </message>
    <message>
        <location line="-85"/>
        <source>noun or verb acting prenominally</source>
        <translation>sustantivo o verbo pronominal</translation>
    </message>
    <message>
        <location line="+90"/>
        <source>transitive verb</source>
        <translation>Verbo transitivo</translation>
    </message>
    <message>
        <location line="-65"/>
        <source>noun, used as a suffix</source>
        <translation>Sustantivo usado como sufijo</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>conjunction</source>
        <translation>conjunción</translation>
    </message>
    <message>
        <location line="-15"/>
        <source>adjective (keiyoushi)</source>
        <translation>Adjetivo (keiyoushi)</translation>
    </message>
    <message>
        <location line="+90"/>
        <source>Ichidan verb - zuru verb (alternative form of -jiru verbs)</source>
        <translation>Verbo Ichidan -  verbo zuru (forma alternativa de los verbos &apos;-jiru&apos;)</translation>
    </message>
    <message>
        <location line="-80"/>
        <source>adverb (fukushi)</source>
        <translation>Adverbio (fukushi)</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>suffix</source>
        <translation>sufijo</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>Godan verb - -aru special class</source>
        <translation>Verbo Godan - clase especial &apos;-aru&apos;</translation>
    </message>
    <message>
        <location line="-55"/>
        <source>auxiliary</source>
        <translation>Auxiliares</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>auxiliary verb</source>
        <translation>Verbo auxiliar</translation>
    </message>
    <message>
        <location line="-10"/>
        <source>adjectival nouns or quasi-adjectives (keiyodoshi)</source>
        <translation>Nombre adjetivo o cuasi-adjetivo (keiyodoshi)</translation>
    </message>
    <message>
        <location line="+67"/>
        <source>Godan verb - Iku/Yuku special class</source>
        <translation>Verbo Godan - clase especial Iku/Yuku</translation>
    </message>
    <message>
        <location line="-38"/>
        <source>Ichidan verb</source>
        <translation>Verbo Ichidan</translation>
    </message>
    <message>
        <location line="-10"/>
        <source>noun, used as a prefix</source>
        <translation>Sustantivo usado como prefijo</translation>
    </message>
    <message>
        <location line="-10"/>
        <source>auxiliary adjective</source>
        <translation>Adjetivo auxiliar</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>numeric</source>
        <translation>Numeral</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>counter</source>
        <translation>Contador</translation>
    </message>
    <message>
        <location line="-10"/>
        <source>pre-noun adjectival (rentaishi)</source>
        <translation>Prenominal (rentaishi)</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Nidan verb with &apos;u&apos; ending (archaic)</source>
        <translation>Verbo Nidan acabado en &apos;-u&apos; (arcaico)</translation>
    </message>
    <message>
        <location line="+53"/>
        <source>suru verb - special class</source>
        <translation>Verbo suru - clase especial</translation>
    </message>
    <message>
        <location line="-68"/>
        <source>noun (common) (futsuumeishi)</source>
        <translation>Sustantivo (común) (futsuumeishi)</translation>
    </message>
    <message>
        <location line="+65"/>
        <source>noun or participle which takes the aux. verb suru</source>
        <translation>Sustantivo o participio que usa el verbo auxiliar suru</translation>
    </message>
    <message>
        <location line="-64"/>
        <source>adverbial noun (fukushitekimeishi)</source>
        <translation>sustantivo adverbiativo (fukushitekimeishi)</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>pronoun</source>
        <translation>pronombre</translation>
    </message>
</context>
<context>
    <name>JMdictPreferences</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictPreferences.cc" line="+24"/>
        <source>Vocabulary entries</source>
        <translation>Entradas de vocabulario</translation>
    </message>
    <message>
        <location filename="../src/gui/jmdict/JMdictPreferences.ui" line="+14"/>
        <source>Form</source>
        <translation>Formulario</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Display</source>
        <translation>Mostrar</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Basic information</source>
        <translation>Información básica</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>JLPT level</source>
        <translation>Nivel JLPT</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Kanji used in main reading</source>
        <translation>Kanjis usados en la lectura principal</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>JMdict ID (only for debugging)</source>
        <translation>ID JMdict (sólo para depuración)</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Additional information</source>
        <translation>Información adicional</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>For transitive verbs, look for the equivalent intransitive entry and vice-versa.</source>
        <translation>Para los verbos transitivos, busque la entrada equivalente intransitivo y vice-versa.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Search for transitive/intransitive verb buddy</source>
        <translation>Buscar la contraparte transitiva/intransitiva del verbo</translation>
    </message>
    <message>
        <location line="+12"/>
        <location line="+24"/>
        <source>Display words that have the same pronunciation.</source>
        <translation>Muestra palabras que tienen una misma escritura o pronunciación.</translation>
    </message>
    <message>
        <location line="-21"/>
        <source>Homophones</source>
        <translation>Homofonías</translation>
    </message>
    <message>
        <location line="+7"/>
        <location line="+24"/>
        <source>Studied only</source>
        <translation>Sólo estudiadas</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Homographs</source>
        <translation>Homografías</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Link to example sentences:</source>
        <translation>Enlace a frases de ejemplo:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>If enabled, display a link that will look for example sentences in your web browser using the selected service</source>
        <translation>Si está habilitado, se muestra un enlace que buscará frases de ejemplo en el navegador web usando el servicio seleccionado</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Disabled</source>
        <translation>Inhabilitado</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Printing</source>
        <translation>Impresión</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>General</source>
        <translation>General</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Header font size:</source>
        <translation>Tamaño de la fuente de la cabecera:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Maximum number of definitions:</source>
        <translation>Número máximo de definiciones:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>All</source>
        <translation>Todas</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Kanji</source>
        <translation>Kanji</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kanji used in word</source>
        <translation>Kanjis usados en la palabra</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Only studied kanji</source>
        <translation>Sólo los kanjis estudiados</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Preview</source>
        <translation>Previsualización</translation>
    </message>
    <message>
        <location line="+142"/>
        <source>Definitions filtering</source>
        <translation>Filtro de definiciones</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Filtered definitions</source>
        <translation>Filtros de definiciones</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Displayed:</source>
        <translation>Mostrando:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Definitions types in this list will be searched for and displayed.</source>
        <translation>Los tipos de definición de esta lista serán buscados y mostrados.</translation>
    </message>
    <message>
        <location line="+27"/>
        <location line="+24"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Filtered:</source>
        <translation>Ocultas:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Definitions types in this list will be ignored when searching and displaying entries.</source>
        <translation>Los tipos de definición de esta lista serán ignorados al buscar y mostrar entradas.</translation>
    </message>
</context>
<context>
    <name>JMdictYesNoTrainer</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictYesNoTrainer.cc" line="+23"/>
        <source>Show &amp;furigana</source>
        <translation>Mostrar &amp;furigana</translation>
    </message>
</context>
<context>
    <name>KanaSelector</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanaSelector.ui" line="+14"/>
        <source>Form</source>
        <translation>Formulario</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Hiragana</source>
        <translation>Hiragana</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Katakana</source>
        <translation>Katakana</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Obsolete</source>
        <translation>Obsoletos</translation>
    </message>
</context>
<context>
    <name>KanaView</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanaView.cc" line="+162"/>
        <source>Select All</source>
        <translation>Seleccionar todos</translation>
    </message>
</context>
<context>
    <name>KanjiComponentWidget</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPopup.cc" line="+100"/>
        <source>%1 (drawn as &quot;%2&quot;)</source>
        <translation>%1 (drawn as &quot;%2&quot;)</translation>
    </message>
</context>
<context>
    <name>KanjiInputPopupAction</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiSelector.cc" line="+398"/>
        <source>Triggers the kanji input panel</source>
        <translation>Activa el panel de entrada de kanjis</translation>
    </message>
</context>
<context>
    <name>KanjiPlayer</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPlayer.cc" line="+50"/>
        <source>Play</source>
        <translation>Reproducir</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pause</source>
        <translation>Pausa</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Stop</source>
        <translation>Parar</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Next stroke</source>
        <translation>Siguiente trazo</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Previous stroke</source>
        <translation>Trazo anterior</translation>
    </message>
</context>
<context>
    <name>KanjiPopup</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPopup.ui" line="+81"/>
        <source>See in detailed view</source>
        <translation>Ver en la vista detallada</translation>
    </message>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPopup.cc" line="+86"/>
        <source>&lt;b&gt;On:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;On:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>&lt;b&gt;Kun:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;Kun:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>&lt;b&gt;Freq:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;Frec:&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>&lt;b&gt;Grade:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;Grado:&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>&lt;b&gt;JLPT:&lt;/b&gt; N%1&lt;br/&gt;</source>
        <translation>&lt;b&gt;JLPT:&lt;/b&gt; N%1&lt;br/&gt;</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>&lt;b&gt;Components:&lt;/b&gt; %2 %1</source>
        <translation>&lt;b&gt;Componentes:&lt;/b&gt; %2 %1</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>&lt;b&gt;Radicals:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;Radicales:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="-28"/>
        <source>&lt;b&gt;Score:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;Puntuación:&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPopup.ui" line="-61"/>
        <source>Form</source>
        <translation>Formulario</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Previous entry</source>
        <translation>Entrada anterior</translation>
    </message>
    <message>
        <location line="+3"/>
        <location line="+20"/>
        <location line="+30"/>
        <location line="+20"/>
        <location line="+23"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location line="-76"/>
        <source>Next entry</source>
        <translation>Entrada siguiente</translation>
    </message>
    <message>
        <location line="+50"/>
        <source>Entry options menu</source>
        <translation>Opciones</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Close this popup</source>
        <translation>Cerrar este popup</translation>
    </message>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPopup.cc" line="-36"/>
        <source>No information about this kanji!</source>
        <translation>¡No hay información sobre este kanji!</translation>
    </message>
</context>
<context>
    <name>KanjiSelector</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiSelector.ui" line="+6"/>
        <source>Radical search</source>
        <translation>Búsqueda de radicales</translation>
    </message>
</context>
<context>
    <name>Kanjidic2Entry</name>
    <message>
        <location filename="../src/core/kanjidic2/Kanjidic2Entry.cc" line="+126"/>
        <source>(var) </source>
        <translation>(var)</translation>
    </message>
</context>
<context>
    <name>Kanjidic2EntryFormatter</name>
    <message>
        <location filename="../src/gui/kanjidic2/Kanjidic2EntryFormatter.cc" line="+338"/>
        <source>&lt;b&gt;Strokes:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;Trazos:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>&lt;b&gt;Frequency:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;Frecuencia:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>&lt;b&gt;Grade:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;Grado:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>&lt;b&gt;4 corner:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;4 esquinas:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="-144"/>
        <source> (%1) </source>
        <translation> (%1) </translation>
    </message>
    <message>
        <location line="+128"/>
        <source>&lt;b&gt;JLPT:&lt;/b&gt; N%1</source>
        <translation>&lt;b&gt;JLPT:&lt;/b&gt; N%1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>&lt;b&gt;Heisig:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;Heisig:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>&lt;b&gt;Unicode:&lt;/b&gt; U+%1</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+47"/>
        <source>(JLPT N%1)</source>
        <translation>(JLPT N%1)</translation>
    </message>
    <message>
        <location line="+43"/>
        <source>On</source>
        <translation>On</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Kun</source>
        <translation>Kun</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Nanori</source>
        <translation>Nanori</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Strokes</source>
        <translation>Trazos</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Frequency</source>
        <translation>Frecuencia</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Grade</source>
        <translation>Grado</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>JLPT</source>
        <translation>JLPT</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Heisig</source>
        <translation>Heisig</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Variations</source>
        <translation>Variaciones</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Variation of</source>
        <translation>Variación de</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Unicode</source>
        <translation>Unicode</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>SKIP</source>
        <translation>SKIP</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>4 corner</source>
        <translation>4 esquinas</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Radicals</source>
        <translation>Radicales</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Components</source>
        <translation>Componentes</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Dictionaries</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="-265"/>
        <source>&lt;b&gt;SKIP:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;SKIP:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location filename="../src/core/kanjidic2/DictionaryDescriptions.cc" line="+2"/>
        <source>&quot;Japanese For Busy People&quot; vols I-III, published by the AJLT. The codes are the volume.chapter.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;The Kanji Way to Japanese Language Power&quot; by Dale Crowley.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;A New Dictionary of Kanji Usage&quot; (Gakken)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;Kodansha Kanji Dictionary&quot;, (2nd Ed. of the NJECD) edited by Jack Halpern.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;Kanji Learners Dictionary&quot; (Kodansha) edited by Jack Halpern.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;Kanji Learners Dictionary&quot; (Kodansha), 2nd edition (2013) edited by Jack Halpern.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;New Japanese-English Character Dictionary&quot;, edited by Jack Halpern.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;Remembering The Kanji&quot; by James Heisig.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;Remembering The Kanji, Sixth Ed.&quot; by James Heisig.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;A Guide To Remembering Japanese Characters&quot; by Kenneth G. Henshall.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;A Guide To Reading and Writing Japanese&quot; 3rd edition, edited by Henshall, Seeley and De Groot.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Japanese Kanji Flashcards, by Max Hodges and Tomoko Okazaki. (Series 1)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;Kanji in Context&quot; by Nishiguchi and Kono.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>the &quot;Kodansha Compact Kanji Guide&quot;.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>codes from Yves Maniette&apos;s &quot;Les Kanjis dans la tete&quot; French adaptation of Heisig.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;Daikanwajiten&quot; compiled by Morohashi. For some kanji two additional attributes are used: m_vol: the volume of the dictionary in which the kanji is found, and m_page: the page number in the volume.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;Modern Reader&apos;s Japanese-English Character Dictionary&quot;, edited by Andrew Nelson (now published as the &quot;Classic&quot; Nelson).</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;The New Nelson Japanese-English Character Dictionary&quot;, edited by John Haig.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;Essential Kanji&quot; by P.G. O&apos;Neill.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;Japanese Names&quot;, by P.G. O&apos;Neill.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;A Guide To Reading and Writing Japanese&quot; edited by Florence Sakade.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;Kanji and Kana&quot; by Spahn and Hadamitzky.</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;Kanji and Kana&quot; by Spahn and Hadamitzky (2011 edition).</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Tuttle Kanji Cards, compiled by Alexander Kask.</source>
        <translation type="unfinished"/>
    </message>
</context>
<context>
    <name>Kanjidic2FilterWidget</name>
    <message>
        <location filename="../src/gui/kanjidic2/Kanjidic2FilterWidget.cc" line="+47"/>
        <source>Stroke count</source>
        <translation>Número de trazos</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Range</source>
        <translation>Rango</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Radicals</source>
        <translation>Radicales</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Components</source>
        <translation>Componentes</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Unicode</source>
        <translation>Unicode</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>SKIP code</source>
        <translation>Código SKIP</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Four corner</source>
        <translation>Cuatro esquinas</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>School grade</source>
        <translation>Grado escolar</translation>
    </message>
    <message>
        <location line="+3"/>
        <location line="+261"/>
        <source>Grade</source>
        <translation>Grado</translation>
    </message>
    <message>
        <location line="-247"/>
        <source>All &amp;Kyouiku kanji</source>
        <translation>Todos los &amp;Kyouiku kanji</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>All &amp;Jouyou kanji</source>
        <translation>Todos los &amp;Jouyou kanji</translation>
    </message>
    <message>
        <location line="+219"/>
        <source>, %1 strokes</source>
        <translation>, %1 trazos</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>, strokes&lt;=%1</source>
        <translation>, trazos&lt;=%1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>, strokes&gt;=%1</source>
        <translation>, trazos&gt;=%1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>, %1-%2 strokes</source>
        <translation>, entre %1 y %2 trazos</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>, unicode: %1</source>
        <translation>, unicode: %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>, skip: %1</source>
        <translation>, skip: %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>, 4c: %1</source>
        <translation>, 4e: %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>, grade: %1</source>
        <translation>, grado: %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <location line="+1"/>
        <source>Kanji</source>
        <translation>Kanji</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Grade:</source>
        <translation>Grado:</translation>
    </message>
</context>
<context>
    <name>Kanjidic2GUIPlugin</name>
    <message>
        <location filename="../src/gui/kanjidic2/Kanjidic2GUIPlugin.cc" line="+49"/>
        <location line="+7"/>
        <source>Invalid</source>
        <translation>Inválido</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>Elementary 1st grade</source>
        <translation>1er grado elemental</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 2nd grade</source>
        <translation>2º grado elemental</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 3rd grade</source>
        <translation>3er grado elemental</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 4th grade</source>
        <translation>4º grado elemental</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 5th grade</source>
        <translation>5º grado elemental</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 6th grade</source>
        <translation>6º grado elemental</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Junior high school</source>
        <translation>Escuela secundaria</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Used for names (1)</source>
        <translation>Usado para nombres (1)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Used for names (2)</source>
        <translation>Usado para nombres (2)</translation>
    </message>
    <message>
        <location line="+33"/>
        <source>Character flashcards</source>
        <translation>Tarjetas de caracteres</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;character, whole study list</source>
        <translation>Del &amp;carácter, toda la lista de estudio</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;character, current set</source>
        <translation>Del &amp;carácter, la selección actual</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>From &amp;meaning, whole study list</source>
        <translation>Del &amp;significado, toda la lista de estudio</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;meaning, current set</source>
        <translation>Del &amp;significado, la selección actual</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>&amp;Reading practice, whole study list</source>
        <translation>Práctica de &amp;lectura; toda la lista de estudio</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kana</source>
        <translation>Kana</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Radical search input</source>
        <translation>Entrada de búsqueda de radicales</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Component search input</source>
        <translation>Entrada de búsqueda de componentes</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Show stroke popup for currently displayed kanji</source>
        <translation>Muestra el popup con los trazos para el kanji que se está mostrando</translation>
    </message>
    <message>
        <location line="+94"/>
        <location line="+18"/>
        <source>Nothing to train</source>
        <translation>Nada para entrenar</translation>
    </message>
    <message>
        <location line="-18"/>
        <location line="+18"/>
        <source>There are no kanji entries in this set to train on.</source>
        <translation>No hay entradas de kanji en este set para entrenar.</translation>
    </message>
    <message>
        <location line="+186"/>
        <source>Open in detailed view...</source>
        <translation>Abrir en la vista detallada...</translation>
    </message>
    <message>
        <location line="+25"/>
        <source>Tracing for %1</source>
        <translation>Escritura de %1</translation>
    </message>
</context>
<context>
    <name>Kanjidic2Preferences</name>
    <message>
        <location filename="../src/gui/kanjidic2/Kanjidic2Preferences.ui" line="+14"/>
        <source>Form</source>
        <translation>Formulario</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Display</source>
        <translation>Mostrar</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Basic information</source>
        <translation>Información básica</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Readings</source>
        <translation>Lecturas</translation>
    </message>
    <message>
        <location line="+7"/>
        <location line="+532"/>
        <source>Unicode</source>
        <translation>Unicode</translation>
    </message>
    <message>
        <location line="-497"/>
        <source>Variations</source>
        <translation>Variaciones</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Original</source>
        <translation>Original</translation>
    </message>
    <message>
        <location line="+14"/>
        <location line="+497"/>
        <source>Frequency</source>
        <translation>Frecuencia</translation>
    </message>
    <message>
        <location line="-469"/>
        <location line="+476"/>
        <source>School grade</source>
        <translation>Nivel escolar</translation>
    </message>
    <message>
        <location line="-483"/>
        <location line="+490"/>
        <source>JLPT level</source>
        <translation>Nivel JLPT</translation>
    </message>
    <message>
        <location line="-458"/>
        <location line="+116"/>
        <source>Components</source>
        <translation>Componentes</translation>
    </message>
    <message>
        <location line="-197"/>
        <location line="+511"/>
        <source># of strokes</source>
        <translation>Número de trazos</translation>
    </message>
    <message>
        <location line="-518"/>
        <location line="+504"/>
        <source>SKIP code</source>
        <translation>SALTEAR código</translation>
    </message>
    <message>
        <location line="-490"/>
        <source>Nanori</source>
        <translation>Nanori</translation>
    </message>
    <message>
        <location line="+28"/>
        <location line="+511"/>
        <source>Four corner code</source>
        <translation>Código de 4 esquinas</translation>
    </message>
    <message>
        <location line="-504"/>
        <location line="+511"/>
        <source>Heisig number</source>
        <translation>Número Heisig</translation>
    </message>
    <message>
        <location line="-487"/>
        <source>Additional information</source>
        <translation>Información adicional</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Radicals</source>
        <translation>Radicales</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Dictionaries</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+11"/>
        <source>Compounds:</source>
        <translation>Componentes:</translation>
    </message>
    <message>
        <location line="+14"/>
        <location line="+25"/>
        <source>Studied only</source>
        <translation>Sólo los estudiados</translation>
    </message>
    <message>
        <location line="-14"/>
        <source>Words using the kanji:</source>
        <translation>Palabras que usan el kanji:</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>Printing</source>
        <translation>Impresión</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kanji information</source>
        <translation>Información del kanji</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Meaning</source>
        <translation>Significado</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Onyomi</source>
        <translation>Onyomi</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Kunyomi</source>
        <translation>Kunyomi</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Only studied components</source>
        <translation>Sólo los componentes estudiados</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>words</source>
        <translation>palabras</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Only studied words</source>
        <translation>Sólo las palabras estudiadas</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Kanji drawing style</source>
        <translation>Estilo de dibujo del kanji</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Handwriting</source>
        <translation>Escritura a mano</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Kanji header size:</source>
        <translation>Tamaño de la cabecera del kanji:</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Print grid</source>
        <translation>Cuadrícula de impresión</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Show stroke numbers</source>
        <translation>Mostrar los números de los trazos</translation>
    </message>
    <message>
        <location line="+26"/>
        <location line="+356"/>
        <source>px</source>
        <translation>px</translation>
    </message>
    <message>
        <location line="-338"/>
        <location line="+464"/>
        <source>Preview</source>
        <translation>Previsualización</translation>
    </message>
    <message>
        <location line="-322"/>
        <source>Tooltip</source>
        <translation>Sugerencia</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Score</source>
        <translation>Puntuación</translation>
    </message>
    <message>
        <location line="+73"/>
        <source>Stroke animation</source>
        <translation>Animación de trazos</translation>
    </message>
    <message>
        <location line="+181"/>
        <source> seconds</source>
        <translation> segundos</translation>
    </message>
    <message>
        <location line="+32"/>
        <source>Kana selector</source>
        <translation>Selector de kanas</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Appearance</source>
        <translation>Apariencia</translation>
    </message>
    <message>
        <location line="-484"/>
        <location line="+280"/>
        <source>Size:</source>
        <translation>Tamaño:</translation>
    </message>
    <message>
        <location line="-339"/>
        <source>Defa&amp;ult font</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+235"/>
        <source>Kan&amp;ji tooltip enabled</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+121"/>
        <source>Drawing speed:</source>
        <translation>Velocidad de dibujado:</translation>
    </message>
    <message>
        <location line="+9"/>
        <location line="+31"/>
        <source>Default</source>
        <translation>Por defecto</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Pause between strokes:</source>
        <translation>Pausa tras cada trazo:</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Show numbers</source>
        <translation>Mostrar números</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Show grid</source>
        <translation>Mostrar cuadrícula</translation>
    </message>
    <message>
        <location line="+39"/>
        <source>Animation playback</source>
        <translation>Reproducción de la animación</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Play animation when the kanji popup appears</source>
        <translation>Reproducir la animación cuando el popup del kanji aparezca</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Loop animation after:</source>
        <translation>Repetir animación después de:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Do not loop</source>
        <translation>No repetir</translation>
    </message>
    <message>
        <location filename="../src/gui/kanjidic2/Kanjidic2Preferences.cc" line="+27"/>
        <source>Character entries</source>
        <translation>Entradas de caracteres</translation>
    </message>
    <message>
        <location line="+43"/>
        <source>Character font</source>
        <translation>Fuente para los caracteres</translation>
    </message>
</context>
<context>
    <name>ListsViewPreferences</name>
    <message>
        <location filename="../src/gui/ListsViewPreferences.ui" line="+17"/>
        <source>Lists display</source>
        <translation>Mostrar listas</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Smooth scrolling</source>
        <translation>Desplazamiento suave</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Preview</source>
        <translation>Previsualización</translation>
    </message>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="+162"/>
        <source>Lists</source>
        <translation>Listas</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="+158"/>
        <source>&amp;Quit</source>
        <translation>&amp;Salir</translation>
    </message>
    <message>
        <location line="-114"/>
        <source>&amp;Program</source>
        <translation>&amp;Programa</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>&amp;Search</source>
        <translation>&amp;Buscar</translation>
    </message>
    <message>
        <location line="-11"/>
        <source>&amp;Help</source>
        <translation>A&amp;yuda</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>P&amp;ractice</source>
        <translation>P&amp;racticar</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>S&amp;aved searches</source>
        <translation>Búsqued&amp;as guardadas</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Lists</source>
        <translation>Listas</translation>
    </message>
    <message>
        <location line="+55"/>
        <source>Ctrl+Q</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+29"/>
        <source>&amp;Manual</source>
        <translation>&amp;Manual</translation>
    </message>
    <message>
        <location line="+45"/>
        <source>&amp;About...</source>
        <translation>&amp;Acerca de...</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Focus results list</source>
        <translation>Foco en la lista de resultados</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Ctrl+O</source>
        <translation>Ctrl+O</translation>
    </message>
    <message>
        <location line="-115"/>
        <source>Preferences...</source>
        <translation>Preferencias...</translation>
    </message>
    <message>
        <location line="+56"/>
        <source>&amp;Report a bug...</source>
        <translation>&amp;Reportar un bug...</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>&amp;Suggest a feature...</source>
        <translation>&amp;Sugerir una mejora...</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Ask a &amp;question...</source>
        <translation>Hacer una &amp;pregunta...</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Support Tagaini Jisho - Make a &amp;donation!</source>
        <translation>Ayuda a Tagaini Jisho - ¡Haz una &amp;donación!</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Focus text search</source>
        <translation>Ir al cuadro de búsqueda</translation>
    </message>
    <message>
        <location line="-122"/>
        <location filename="../src/gui/MainWindow.cc" line="+235"/>
        <location line="+7"/>
        <source>Export user data...</source>
        <translation>Exportar datos de usuario...</translation>
    </message>
    <message>
        <location line="+9"/>
        <location filename="../src/gui/MainWindow.cc" line="+6"/>
        <source>Import user data...</source>
        <translation>Importar datos de usuario...</translation>
    </message>
    <message>
        <location line="-123"/>
        <source>Tagaini Jisho</source>
        <translation>Tagaini Jisho</translation>
    </message>
    <message>
        <location line="+80"/>
        <source>Search</source>
        <translation>Búsqueda</translation>
    </message>
    <message>
        <location line="+75"/>
        <source>&amp;Save current search...</source>
        <translation>&amp;Guardar la búsqueda actual...</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>&amp;New folder...</source>
        <translation>&amp;Nueva carpeta...</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>&amp;Organize saved searches...</source>
        <translation>&amp;Organizar las búsquedas guardadas</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Whole study list train &amp;settings...</source>
        <translation>&amp;Preferencias de entrenamiento...</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cc" line="-55"/>
        <source>Auto-search on clipboard content</source>
        <translation>Auto-buscar en el contenido del portapapeles</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Please consider supporting Tagaini Jisho</source>
        <translation>Por favor, considere ayudar a Tagaini Jisho</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You have now been using Tagaini Jisho for a short while. If you like it, please show your support by making a donation that expresses your appreciation of this software. While Tagaini is free, its development has a cost and your support will have a positive influence on its future. You can donate via Paypal or credit card.

This message is a one-time reminder and will not appear anymore - you can still donate later by choosing the corresponding option in the Help menu.</source>
        <translation>Ha estado usando Tagaini Jisho durante un tiempo. Si le ha gustado, por favor, demuestre su apoyo realizando una donación que exprese su aprecio por este programa. Tagaini es gratuito pero su desarrollo tiene un coste y su ayuda tendrá una influencia positiva en su futuro. Puede donar usando Paypal o tarjeta de crédito.

Este mensaje es un recordatorio que no volverá a aparecer, pero puede donar mas tarde haciendo clic en la opción del menú Ayuda.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Donate!</source>
        <translation>¡Donar!</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Nah</source>
        <translation>Nah</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Error: Could not export database! Please verify that you have write permissions on the target file.</source>
        <translation>Error: ¡No se puede exportar la base de datos! Por favor verifica que tienes permisos de escritura para el archivo destinatario.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>About to replace user data!</source>
        <translation>¡A punto de reemplazar los datos del usuario!</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>&lt;b&gt;WARNING:&lt;/b&gt; All the user data (study list, tags, notes, training, ...) is going to be replaced by the selected user data file. Are you &lt;b&gt;sure&lt;/b&gt; you want to do this?</source>
        <translation>&lt;b&gt;ALERTA:&lt;/b&gt; Todos los datos de usuario (listas de estudio, etiquetas, notas, entrenamiento, ...) serán reemplazados por el archivo de datos de usuario seleccionado. ¿Está &lt;b&gt;seguro&lt;/b&gt; de querer hacer esto?</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Please restart Tagaini Jisho</source>
        <translation>Por favor, reinicie Tagaini Jisho</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The imported data will be available the next time you start Tagaini Jisho. All changes performed from now will be lost. Do you want to exit Tagaini Jisho now?</source>
        <translation>Los datos importados estarán disponibles la próxima vez que inicie Tagaini Jisho. Todos los cambios realizados a partir de ahora se perderán. ¿Quiere salir Tagaini Jisho ahora?</translation>
    </message>
    <message>
        <location line="+113"/>
        <source>A development update is available!</source>
        <translation>¡Está disponible una actualización de desarrollo!</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Development version %1 of Tagaini Jisho is available. Do you want to download it now?</source>
        <translation>La versión en desarrollo %1 de Tagaini Jisho está disponible. ¿Quiere descargarla ahora?</translation>
    </message>
    <message>
        <location line="+54"/>
        <source>Save current search here...</source>
        <translation>Guardar la búsqueda actual aquí...</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Save current search</source>
        <translation>Guardar la búsqueda actual</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Please enter a name for this search:</source>
        <translation>Por favor, introduzca un nombre para esta búsqueda:</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unnamed</source>
        <translation>Sin nombre</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>New folder</source>
        <translation>Nueva carpeta</translation>
    </message>
    <message>
        <location line="-34"/>
        <source>Create new folder here...</source>
        <translation>Crear una nueva carpeta aquí...</translation>
    </message>
    <message>
        <location line="+34"/>
        <source>Please enter a name for this folder</source>
        <translation>Por favor, introduzca un nombre para esta carpeta</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unnamed folder</source>
        <translation>Carpeta sin nombre</translation>
    </message>
    <message>
        <location line="-106"/>
        <source>An update is available!</source>
        <translation>¡Hay una actualización disponible!</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Version %1 of Tagaini Jisho is available. Do you want to download it now?</source>
        <translation>La versión %1 de Tagaini Jisho está disponible. ¿Quiere descargarla ahora?</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+15"/>
        <source>Let&apos;s go!</source>
        <translation>¡Vamos allá!</translation>
    </message>
    <message>
        <location line="-13"/>
        <location line="+15"/>
        <source>Maybe later</source>
        <translation>Más tarde</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="+69"/>
        <source>Ctrl+L</source>
        <translation>Ctrl+L</translation>
    </message>
</context>
<context>
    <name>MultiStackedWidget</name>
    <message>
        <location filename="../src/gui/MultiStackedWidget.cc" line="+118"/>
        <source>Ctrl+%1</source>
        <extracomment>Shortcut for popping up/down an extender</extracomment>
        <translation>Ctrl+%1</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Ctrl+0</source>
        <extracomment>Shortcut for hiding all extenders</extracomment>
        <translation>Ctrl+0</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Reset this filter</source>
        <translation>Eliminar este filtro</translation>
    </message>
</context>
<context>
    <name>NotesFilterWidget</name>
    <message>
        <location filename="../src/gui/NotesFilterWidget.cc" line="+50"/>
        <source>Notes</source>
        <translation>Notas</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Has note</source>
        <translation>Contiene notas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Note contains %1</source>
        <translation>Nota contiene %1</translation>
    </message>
</context>
<context>
    <name>PreferencesFontChooser</name>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="+156"/>
        <source>Change...</source>
        <translation>Cambiar...</translation>
    </message>
    <message>
        <location line="-5"/>
        <source>%1:</source>
        <translation>%1:</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Default</source>
        <translation>Por defecto</translation>
    </message>
</context>
<context>
    <name>PreferencesWindow</name>
    <message>
        <location filename="../src/gui/PreferencesWindow.ui" line="+14"/>
        <source>Preferences</source>
        <translation>Preferencias</translation>
    </message>
</context>
<context>
    <name>RadicalSearchWidget</name>
    <message>
        <location filename="../src/gui/kanjidic2/RadicalSearchWidget.ui" line="+14"/>
        <source>Radical search</source>
        <translation>Búsqueda de radicales</translation>
    </message>
</context>
<context>
    <name>ReadingTrainer</name>
    <message>
        <location filename="../src/gui/ReadingTrainer.ui" line="+14"/>
        <location filename="../src/gui/ReadingTrainer.cc" line="+41"/>
        <source>Reading practice</source>
        <translation>Práctica de lectura</translation>
    </message>
    <message>
        <location filename="../src/gui/ReadingTrainer.cc" line="+5"/>
        <source>Show &amp;meaning</source>
        <translation>&amp;Mostrar significado</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Preparing training session</source>
        <translation>Preparando sesión de entrenamiento</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Preparing training session, please wait...</source>
        <translation>Preparando sesión de entrenamiento, espere por favor...</translation>
    </message>
    <message>
        <location line="+57"/>
        <source>No matching entries found</source>
        <translation>No se han encontrado entradas coincidentes</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unable to find any entry eligible for reading practice. Entries eligible for this training mode are studied vocabulary entries for which all kanji are also studied, and that match the train settings. Please add entries or modify the train settings accordingly if you want to practice this mode.</source>
        <translation>Imposible encontrar entradas para la práctica de lectura. Las entradas elegibles en este modo son aquellas entradas en las cuales todos sus kanjis  están bajo estudio y satisfacen los ajustes de entrenamiento. Por favor agregue entradas o modifique los ajustes si quiere practicar en este modo.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>There are no more entries to train for the current train settings.</source>
        <translation>No hay más entradas para entrenar para la configuración de entrenamiento actual.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No more entries to train</source>
        <translation>No hay más entradas para entrenar</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>&lt;font color=&quot;green&quot;&gt;Correct!&lt;/font&gt;</source>
        <translation>&lt;font color=&quot;green&quot;&gt;¡Correcto!&lt;/font&gt;</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>&lt;font color=&quot;red&quot;&gt;Error!&lt;/font&gt;</source>
        <translation>&lt;font color=&quot;red&quot;&gt;¡Incorrecto!&lt;/font&gt;</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>Correct: %1, Wrong: %2, Total: %3</source>
        <translation>Correcto: %1, Incorrecto: %2, Total: %3</translation>
    </message>
    <message>
        <location filename="../src/gui/ReadingTrainer.ui" line="+9"/>
        <source>Type the correct reading for this word:</source>
        <translation>Escriba la lectura correcta de esta palabra:</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>&amp;Next</source>
        <translation>&amp;Siguiente</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>&amp;Ok!</source>
        <translation>&amp;Comprobar</translation>
    </message>
</context>
<context>
    <name>RelativeDate</name>
    <message>
        <location filename="../src/core/RelativeDate.cc" line="+99"/>
        <source>today</source>
        <translation>hoy</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>yesterday</source>
        <translation>ayer</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 days ago</source>
        <translation>hace %1 días</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>this week</source>
        <translation>esta semana</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>last week</source>
        <translation>la semana pasada</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 weeks ago</source>
        <translation>hace %1 semanas</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>this month</source>
        <translation>este mes</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>last month</source>
        <translation>el mes pasado</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 months ago</source>
        <translation>hace %1 meses</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>this year</source>
        <translation>este año</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>last year</source>
        <translation>el año pasado</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 years ago</source>
        <translation>hace %1 años</translation>
    </message>
</context>
<context>
    <name>RelativeDateEdit</name>
    <message>
        <location filename="../src/gui/RelativeDateEdit.cc" line="+163"/>
        <source>Not set</source>
        <translation>No definida</translation>
    </message>
</context>
<context>
    <name>RelativeDatePopup</name>
    <message>
        <location line="-130"/>
        <source>Not set</source>
        <translation>No definida</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>days ago</source>
        <translation>días atrás</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>weeks ago</source>
        <translation>semanas atrás</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>months ago</source>
        <translation>meses atrás</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>years ago</source>
        <translation>años atrás</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ok</source>
        <translation>Aceptar</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Relative date:</source>
        <translation>Fecha relativa:</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Absolute date:</source>
        <translation>Fecha absoluta:</translation>
    </message>
</context>
<context>
    <name>ResultsView</name>
    <message>
        <location filename="../src/gui/ResultsView.cc" line="+50"/>
        <source>Select All</source>
        <translation>Seleccionar todo</translation>
    </message>
</context>
<context>
    <name>ResultsViewPreferences</name>
    <message>
        <location filename="../src/gui/ResultsViewPreferences.ui" line="+17"/>
        <source>Results display</source>
        <translation>Mostrar resultados</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Smooth scrolling</source>
        <translation>Desplazamiento suave</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Results sorting:</source>
        <translation>Orden de los resultados:</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>By relevance</source>
        <translation>Por relevancia</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>By score, then relevance</source>
        <translation>Por puntuación y después relevancia</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Preview</source>
        <translation>Previsualización</translation>
    </message>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="-187"/>
        <source>Results view</source>
        <translation>Vista de resultados</translation>
    </message>
</context>
<context>
    <name>ResultsViewWidget</name>
    <message>
        <location filename="../src/gui/ResultsViewWidget.cc" line="+43"/>
        <source>Searching...</source>
        <translation>Buscando...</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>%1 Results</source>
        <translation>%1 resultados</translation>
    </message>
</context>
<context>
    <name>SavedSearchesOrganizer</name>
    <message>
        <location filename="../src/gui/SavedSearchesOrganizer.ui" line="+14"/>
        <source>Organize saved searches</source>
        <translation>Organizar las búsquedas guardadas</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>You can organize your saved searches here. Double-click to change the search&apos;s label, drag and drop to move or right-click for other options, including deletion.</source>
        <translation>Puede organizar las búsquedas guardadas aquí. Doble clic para cambiar la etiqueta de la búsqueda, arrastrar y soltar para mover , o clic derecho para otras opciones, incluido eliminar.</translation>
    </message>
</context>
<context>
    <name>SavedSearchesTreeWidget</name>
    <message>
        <location filename="../src/gui/SavedSearchesOrganizer.cc" line="+145"/>
        <source>Delete</source>
        <translation>Eliminar</translation>
    </message>
    <message>
        <location line="+36"/>
        <source>Confirm deletion</source>
        <translation>Confirmar eliminación</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Are you sure you want to delete the selected search/folder?</source>
        <translation>¿Está seguro de querer eliminar la búsqueda/carpeta seleccionada?</translation>
    </message>
</context>
<context>
    <name>SearchWidget</name>
    <message>
        <location filename="../src/gui/SearchWidget.ui" line="+67"/>
        <source>Previous search</source>
        <translation>Búsqueda anterior</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Next search</source>
        <translation>Búsqueda siguiente</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>&amp;Reset search...</source>
        <translation>&amp;Limpiar búsqueda...</translation>
    </message>
</context>
<context>
    <name>ShowUsedInJob</name>
    <message>
        <location filename="../src/gui/kanjidic2/Kanjidic2EntryFormatter.cc" line="+323"/>
        <source>Direct compounds</source>
        <translation>Componentes directos</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>All compounds</source>
        <translation>Todos los componentes</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>(+)</source>
        <translation>(+)</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Make a new search using only this filter</source>
        <translation>Realiza una nueva búsqueda usando sólo este filtro</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Add this filter to the current search</source>
        <translation>Añade este filtro a la búsqueda actual</translation>
    </message>
</context>
<context>
    <name>ShowUsedInWordsJob</name>
    <message>
        <location line="+10"/>
        <source>Seen in</source>
        <translation>Visto en</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>All words using this kanji</source>
        <translation>Todas las palabras que usan este kanji</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>(+)</source>
        <translation>(+)</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Make a new search using only this filter</source>
        <translation>Realiza una nueva búsqueda usando sólo este filtro</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Add this filter to the current search</source>
        <translation>Añade este filtro a la búsqueda actual</translation>
    </message>
</context>
<context>
    <name>StudyFilterWidget</name>
    <message>
        <location filename="../src/gui/StudyFilterWidget.cc" line="+30"/>
        <source>Score</source>
        <translation>Puntuación</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Min</source>
        <translation>Mín</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Max</source>
        <translation>Máx</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Study date</source>
        <translation>Fecha de estudio</translation>
    </message>
    <message>
        <location line="+24"/>
        <location line="+30"/>
        <location line="+20"/>
        <source>since</source>
        <translation>desde el</translation>
    </message>
    <message>
        <location line="-49"/>
        <location line="+30"/>
        <location line="+20"/>
        <source>until</source>
        <translation>hasta el</translation>
    </message>
    <message>
        <location line="-45"/>
        <source>Last trained</source>
        <translation>Última vez entrenado</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Last mistaken</source>
        <translation>Última vez fallido</translation>
    </message>
    <message>
        <location line="+114"/>
        <source> since %1</source>
        <translation> desde el %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <location line="+13"/>
        <location line="+10"/>
        <source> until %1</source>
        <translation> hasta el %1</translation>
    </message>
    <message>
        <location line="-20"/>
        <source>, score:[%1-%2]</source>
        <translation>, puntuación: [%1-%2]</translation>
    </message>
    <message>
        <location line="-98"/>
        <source>Any</source>
        <translation>Cualquiera</translation>
    </message>
    <message>
        <location line="+3"/>
        <location line="+85"/>
        <source>Studied</source>
        <translation>Estudiado</translation>
    </message>
    <message>
        <location line="-83"/>
        <location line="+116"/>
        <source>Not studied</source>
        <translation>No estudiado</translation>
    </message>
    <message>
        <location line="-112"/>
        <source>Study status:</source>
        <translation>Estado de estudio:</translation>
    </message>
    <message>
        <location line="+94"/>
        <source>, trained since %1</source>
        <translation>, entrenado desde el %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>, trained until %1</source>
        <translation>, entrenado hasta el %1</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>, mistaken since %1</source>
        <translation>, fallido desde el %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>, mistaken until %1</source>
        <translation>, fallido hasta el %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Study</source>
        <translation>Estudiando</translation>
    </message>
</context>
<context>
    <name>TagsDialogs</name>
    <message>
        <location filename="../src/gui/TagsDialogs.cc" line="+175"/>
        <source>Set tags for %1</source>
        <translation>Editar etiquetas de %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Set tags</source>
        <translation>Editar etiquetas</translation>
    </message>
    <message>
        <location line="+1"/>
        <location line="+35"/>
        <source>these entries:</source>
        <translation>estas entradas:</translation>
    </message>
    <message>
        <location line="-35"/>
        <location line="+35"/>
        <source>this entry:</source>
        <translation>esta entrada:</translation>
    </message>
    <message>
        <location line="-35"/>
        <source>Enter a space-separated list of tags for </source>
        <translation>Introduzca una lista de etiquetas separadas por espacios para </translation>
    </message>
    <message>
        <location line="+5"/>
        <location line="+35"/>
        <source>Invalid tags</source>
        <translation>Etiquetas inválidas</translation>
    </message>
    <message>
        <location line="-35"/>
        <location line="+35"/>
        <source>&lt;/p&gt;&lt;p&gt;Accepted characters are letters, digits and non-quoting punctuation.&lt;/p&gt;</source>
        <translation>&lt;/p&gt;&lt;p&gt;Los caracteres aceptados son letras, números y signos de puntuación excepto citas.&lt;/p&gt;</translation>
    </message>
    <message>
        <location line="-35"/>
        <location line="+35"/>
        <source>&lt;p&gt;These tags have invalid characters: </source>
        <translation>&lt;p&gt;Estas etiquetas contienen caracteres inválidos: </translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Add tags to %1</source>
        <translation>Añadir etiquetas a %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Add tags</source>
        <translation>Añadir etiquetas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Enter a space-separated list of tags to add to </source>
        <translation>Introduzca una lista de etiquetas separadas por espacios para añadir a </translation>
    </message>
</context>
<context>
    <name>TagsFilterWidget</name>
    <message>
        <location filename="../src/gui/TagsFilterWidget.cc" line="+32"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location line="+6"/>
        <location line="+62"/>
        <source>Untagged</source>
        <translation>No etiquedado</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Tags</source>
        <translation>Etiquetas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Tagged %1</source>
        <translation>Etiquetas: %1</translation>
    </message>
</context>
<context>
    <name>TextFilterWidget</name>
    <message>
        <location filename="../src/gui/TextFilterWidget.cc" line="+42"/>
        <source>Romaji search</source>
        <translation>Búsqueda con romaji</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>If checked, text typed in romaji will be converted to kana and searched as such.</source>
        <translation>Si está seleccionado, se realizará la búsqueda con el texto escrito convertido a kanas.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Text search</source>
        <translation>Búsqueda de texto</translation>
    </message>
</context>
<context>
    <name>TrainSettings</name>
    <message>
        <location filename="../src/gui/TrainSettings.cc" line="+66"/>
        <source>Entries appear totally randomly.</source>
        <translation>Las entradas aparecen al azar.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Entries with a low score are likely to appear first.</source>
        <translation>Las entradas con una baja puntuación tienden a aparecer primero.</translation>
    </message>
    <message>
        <location filename="../src/gui/TrainSettings.ui" line="+14"/>
        <source>Train settings</source>
        <translation>Configuración de entrenamiento</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Date of last training</source>
        <translation>Fecha del último entrenamiento</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Before</source>
        <translation>Antes</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>After</source>
        <translation>Después</translation>
    </message>
    <message>
        <location line="+10"/>
        <location line="+7"/>
        <source>PushButton</source>
        <translation>PushButton</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Score</source>
        <translation>Puntuación</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>between</source>
        <translation>entre</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>and</source>
        <translation>y</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Sort bias</source>
        <translation>Orden</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Bias:</source>
        <translation>Ordenar:</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>No bias</source>
        <translation>Sin ordenar</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>By score</source>
        <translation>Por puntuación</translation>
    </message>
</context>
<context>
    <name>YesNoTrainer</name>
    <message>
        <location filename="../src/gui/YesNoTrainer.cc" line="+44"/>
        <source>Answer (&amp;1)</source>
        <translation>Respuesta (&amp;1)</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Correct! (&amp;2)</source>
        <translation>¡Correcto! (&amp;2)</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Wrong... (&amp;3)</source>
        <translation>Incorrecto... (&amp;3)</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Skip (&amp;4)</source>
        <translation>Saltar (&amp;4)</translation>
    </message>
    <message>
        <location line="+108"/>
        <source>No entries to train</source>
        <translation>No hay entradas para entrenar</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>There are no entries to train as of now. Please add entries to your study list or check the train settings.</source>
        <translation>No hay entradas para estudiar por ahora. Por favor, añade entradas a tu lista de estudio o mira las opciones de estudio.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>All the matching entries have been trained.</source>
        <translation>Todas las entradas han sido estudiadas.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No more entries to train</source>
        <translation>No hay mas entradas para entrenar</translation>
    </message>
    <message>
        <location line="-47"/>
        <source>Correct: %1, Wrong: %2, Total: %3</source>
        <translation>Correcto: %1, Incorrecto: %2, Total: %3</translation>
    </message>
</context>
<context>
    <name>main.cc</name>
    <message>
        <location filename="../src/gui/main.cc" line="+161"/>
        <source>Data migrated</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+0"/>
        <source>Welcome to Tagaini Jisho %1! Your user data has successfully been migrated from your old version of Tagaini and you should find it as it was.

If you still have an old version of Tagaini installed, please note that changes made in Tagaini %1 will not be visible on it and vice-versa. It is recommended that you uninstall your old Tagaini version after checking that everything is working here.</source>
        <translation type="unfinished"/>
    </message>
</context>
</TS>