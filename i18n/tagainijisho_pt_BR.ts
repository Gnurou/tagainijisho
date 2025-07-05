<?xml version="1.0" ?><!DOCTYPE TS><TS version="2.1" language="pt_BR" sourcelanguage="en">
<context>
    <name>AboutDialog</name>
    <message>
        <location filename="../src/gui/AboutDialog.ui" line="+14"/>
        <source>About Tagaini Jisho</source>
        <translation>Sobre o Tagaini Jisho</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Tagaini Jisho</source>
        <translation>Tagaini Jisho</translation>
    </message>
    <message>
        <location line="+29"/>
        <source>About</source>
        <translation>Sobre</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Authors &amp;&amp; contributors</source>
        <translation>Autores e contribuidores</translation>
    </message>
</context>
<context>
    <name>BatchHandler</name>
    <message>
        <location filename="../src/gui/BatchHandler.cc" line="+26"/>
        <source>Marking entries...</source>
        <translation>Marcando entradas...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Abort</source>
        <translation>Abortar</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Please wait...</source>
        <translation>Por favor, aguarde...</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Cannot start transaction</source>
        <translation>Impossível iniciar transação</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Error while trying to start database transaction.</source>
        <translation>Erro ao tentar iniciar transação do banco de dados</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Cannot commit transaction</source>
        <translation>Impossível executar transação</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Error while trying to commit database transaction.</source>
        <translation>Erro ao tentar executar transação da base de dados</translation>
    </message>
</context>
<context>
    <name>DataPreferences</name>
    <message>
        <location filename="../src/gui/DataPreferences.ui" line="+17"/>
        <source>Erase user data</source>
        <translation>Apagar dados do usuário</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Pushing this button will completely erase all your user data. This includes study list, tags, notes, scores, basically everything you did. After pushing this button, Tagaini Jisho will exit. Upon restart, you will be back to a fresh, blank database.</source>
        <translation>Ao apertar este botão, todos os seus dados de usuário serão completamente apagados. Isso inclui a lista de estudos, etiquetas, notas, pontuações, basicamente tudo que você fez. Depois de apertar este botão, o Tagaini Jisho irá fechar. Ao reiniciar, você terá um banco de dados novo e em branco.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Reset user data</source>
        <translation>Reiniciar dados de usuário</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>User database file:</source>
        <translation>Arquivo do banco de dados do usuário:</translation>
    </message>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="+497"/>
        <source>User data</source>
        <translation>Dados do usuário</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Really erase user data?</source>
        <translation>Deseja realmente apagar os dados de usuário?</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>This will erase all your user data. Are you sure you want to do this?</source>
        <translation>Isto irá apagar todos os seus dados de usuário. Você tem certeza que quer fazer isso?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>REALLY erase it?</source>
        <translation>REALMENTE apagar?</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Are you absolutely, positively, definitely sure?</source>
        <translation>Você tem certeza absoluta, total e definitiva?</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>No user database file</source>
        <translation>Sem banco de dados do usuário.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The user database file has already been erased before.</source>
        <translation>O banco de dados do usuário já foi apagado antes.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Cannot erase user database</source>
        <translation>Impossível apagar dados do usuário.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unable to erase user database. Please see what is wrong and try to do it manually.</source>
        <translation>Impossível apagar dados do usuário. Por favor, verifique o que está errado e tente fazê-lo manualmente.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>User data erased</source>
        <translation>Dados do usuário apagados.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>User data has been erased. Tagaini Jisho will now exit. You will be back to a blank database when it is restarted.</source>
        <translation>Os dados do usuário foram apagados. O Tagaini Jisho irá fechar agora. Ao reiniciá-lo, você terá um banco de dados em branco.</translation>
    </message>
</context>
<context>
    <name>Database</name>
    <message>
        <location filename="../src/core/Database.cc" line="+316"/>
        <source>Error while upgrading user database: %1</source>
        <translation>Erro ao atualizar banco de dados: %1</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Wrong user database version: expected %1, got %2.</source>
        <translation>Versão incorreta do banco de dados: versão necessária %1, versão atual %2.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Cannot create user database: %1</source>
        <translation>Impossível criar banco de dados do usuário: %1</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Cannot open database: %1</source>
        <translation>Impossível abrir banco de dados: %1</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Temporary database fallback failed. The program will now exit.</source>
        <translation>Restauração para banco de dados temporário falhou. O programa irá fechar agora.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Tagaini is working on a temporary database. This allows the program to work, but user data is unavailable and any change will be lost upon program exit. If you corrupted your database file, please recreate it from the preferences.</source>
        <translation>O Tagaini está utilizando um banco de dados temporário. Isto permite que o programa funcione, mas os dados do usuário estão indisponíveis e qualquer alteração será perdida ao fechar o programa. Se você corrompeu o seu arquivo de base de dados, por favor recrie-o nas preferências.</translation>
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
        <translation>Próxima entrada</translation>
    </message>
</context>
<context>
    <name>DetailedViewPreferences</name>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="-108"/>
        <source>Detailed view</source>
        <translation>Visão detalhada</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Default text</source>
        <translation>Texto padrão</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kana header</source>
        <translation>Cabeçalho de kana</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kanji header</source>
        <translation>Leitor de kanji</translation>
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
        <translation>Geral</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Smooth scrolling</source>
        <translation>Rolagem suave</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Show JLPT level in short descriptions</source>
        <translation>Mostrar nível JLPT nas descrições curtas</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Fonts</source>
        <translation>Fontes</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Preview</source>
        <translation>Visualização</translation>
    </message>
</context>
<context>
    <name>EditEntryNotesDialog</name>
    <message>
        <location filename="../src/gui/EditEntryNotesDialog.cc" line="+86"/>
        <source>Notes for %1</source>
        <translation>Anotações para %1</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Existing notes:</source>
        <translation>Anotações existentes:</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>&amp;New note</source>
        <translation>&amp;Nova anotação</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>&amp;Delete note</source>
        <translation>&amp;Excluir anotação</translation>
    </message>
</context>
<context>
    <name>EntriesPrinter</name>
    <message>
        <location filename="../src/gui/EntriesPrinter.cc" line="+66"/>
        <source>Preparing print job...</source>
        <translation>Preparando trabalho de impressão</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Abort</source>
        <translation>Abortar</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Printing...</source>
        <translation>Imprimindo...</translation>
    </message>
    <message>
        <location line="+89"/>
        <source>Print preview</source>
        <translation>Visualização de impressão</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Booklet print preview</source>
        <translation>Visualização de impressão de folheto</translation>
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
        <translation>Imprimir &amp;visualização...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Print &amp;booklet...</source>
        <translation>Imprimir &amp;folheto...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Export as &amp;HTML...</source>
        <translation>Exportar como &amp;HTML...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Booklet pre&amp;view...</source>
        <translation>V&amp;isualização de folheto...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Export as &amp;TSV...</source>
        <translation>Exportar como &amp;TSV...</translation>
    </message>
    <message>
        <location line="+57"/>
        <source>Abort</source>
        <translation>Abortar</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Selecting entries...</source>
        <translation>Selecionando entradas...</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Please wait...</source>
        <translation>Por favor, aguarde...</translation>
    </message>
    <message>
        <location line="+259"/>
        <source>Print preview</source>
        <translation>Visualização de impressão</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Booklet print</source>
        <translation>Impressão de folheto</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Booklet print preview</source>
        <translation>Visualização de impressão de folheto</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Export to tab-separated file...</source>
        <translation>Exportar para arquivo separado por tabulação</translation>
    </message>
    <message>
        <location line="+4"/>
        <location line="+47"/>
        <source>Cannot write file</source>
        <translation>Impossível criar arquivo</translation>
    </message>
    <message>
        <location line="-47"/>
        <source>Unable to write file %1.</source>
        <translation>Impossível criar arquivo %1.</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Error writing file</source>
        <translation>Erro ao criar arquivo</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Error while writing file %1.</source>
        <translation>Erro ao criar arquivo %1.</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Export to HTML flashcard file...</source>
        <translation>Exportar como arquivo HTML de cartões (flashcards)...</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Unable to write file %1!</source>
        <translation>Impossível criar arquivo %1!</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Cannot open template file</source>
        <translation>Impossível abrir arquivo modelo</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unable to open template file!</source>
        <translation>Impossível abrir arquivo de modelo!</translation>
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
        <translation>Uma &amp;linha</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>&amp;Two lines</source>
        <translation>&amp;Duas linhas</translation>
    </message>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="-196"/>
        <source>Main writing</source>
        <translation>Escrita principal</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Readings and alternate writings</source>
        <translation>Leituras e escritas alternativas</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Definitions</source>
        <translation>Definições</translation>
    </message>
</context>
<context>
    <name>EntryDragButton</name>
    <message>
        <location filename="../src/gui/ToolBarDetailedView.cc" line="+29"/>
        <source>Drag the currently displayed entry</source>
        <translation>Arrastar a entrada sendo mostrada atualmente</translation>
    </message>
</context>
<context>
    <name>EntryFormatter</name>
    <message>
        <location filename="../src/gui/EntryFormatter.cc" line="+57"/>
        <source>Cannot find detailed view HTML file!</source>
        <translation>Impossível encontrar arquivo HTML de exibição detalhada!</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Cannot find detailed view CSS file!</source>
        <translation>Impossível encontrar arquivo CSS de visão detalhada!</translation>
    </message>
    <message>
        <location line="+181"/>
        <source>Root list</source>
        <translation>Lista raíz</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>Studied since %1.</source>
        <translation>Estudado desde %1.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Score: %1.</source>
        <translation>Pontuação: %1.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Last trained on %1.</source>
        <translation>Treinado pela última vez em %1.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Last mistaken on %1.</source>
        <translation>Errado pela última vez em %1.</translation>
    </message>
</context>
<context>
    <name>EntryListView</name>
    <message>
        <location filename="../src/gui/EntryListView.cc" line="+36"/>
        <source>Delete</source>
        <translation>Excluir</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>New list...</source>
        <translation>Nova lista...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Go up</source>
        <translation>Ir para cima</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Rename list...</source>
        <translation>Renomear lista...</translation>
    </message>
    <message>
        <location line="+93"/>
        <source>Unable to create list</source>
        <translation>Impossível criar lista</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>A database error occured while trying to add the list.</source>
        <translation>Um erro de banco de dados ocorreu ao tentar adicionar a lista.</translation>
    </message>
    <message>
        <location line="+39"/>
        <source>Confirm deletion</source>
        <translation>Confirmar exclusão</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>This will delete the selected lists items and lists, including all their children. Continue?</source>
        <translation>Isto irá excluir as listas e itens selecionados, incluindo todos os sub-itens. Continuar?</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Removal failed</source>
        <translation>Falha ao excluir</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>A database error has occured while trying to remove the selected items:

%1

 Some of them may be remaining.</source>
        <translation>Um erro de banco de dados ocorreu ao tentar remover os seguintes itens:

%1

 Alguns deles podem não ter sido excluídos.</translation>
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
        <translation>Adicionar à lista de &amp;estudos</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Remove from &amp;study list</source>
        <translation>&amp;Remover da lista de estudos</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Already &amp;known</source>
        <translation>Já &amp;sabido</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>&amp;Reset score</source>
        <translation>&amp;Zerar pontuação</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Set &amp;tags...</source>
        <translation>Edi&amp;tar etiquetas...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>&amp;Add tags...</source>
        <translation>A&amp;dicionar etiquetas...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Edit &amp;notes...</source>
        <translation>Editar a&amp;notações</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Recently added tags...</source>
        <translation>Etiquetas recentemente adicionadas...</translation>
    </message>
    <message>
        <location line="+87"/>
        <location line="+6"/>
        <source>Copy &quot;%1&quot; to clipboard</source>
        <translation>Copiar &quot;%1&quot; para a área de transferência</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Add &quot;%1&quot;</source>
        <translation>Adicionar &quot;%1&quot;</translation>
    </message>
</context>
<context>
    <name>EntryNotesModel</name>
    <message>
        <location filename="../src/gui/EditEntryNotesDialog.cc" line="-65"/>
        <source>Notes</source>
        <translation>Anotações</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>&lt;New note&gt;</source>
        <translation>&lt;Nova anotação&gt;</translation>
    </message>
</context>
<context>
    <name>EntryTypeFilterWidget</name>
    <message>
        <location filename="../src/gui/EntryTypeFilterWidget.cc" line="+33"/>
        <source>All</source>
        <translation>Todos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Vocabulary</source>
        <translation>Vocabulário</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Characters</source>
        <translation>Caracteres</translation>
    </message>
    <message>
        <location filename="../src/gui/EntryTypeFilterWidget.h" line="+43"/>
        <source>Entry type filter</source>
        <translation>Filtro de tipo de entrada</translation>
    </message>
</context>
<context>
    <name>FindHomographsJob</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictEntryFormatter.cc" line="+625"/>
        <source>Homographs</source>
        <translation>Homógrafos</translation>
    </message>
</context>
<context>
    <name>FindHomophonesJob</name>
    <message>
        <location line="-23"/>
        <source>Homophones</source>
        <translation>Homófonos</translation>
    </message>
</context>
<context>
    <name>FindVerbBuddyJob</name>
    <message>
        <location line="-15"/>
        <source>Transitive buddy</source>
        <translation>Par transitivo</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Intransitive buddy</source>
        <translation>Par intransitivo</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Buddy</source>
        <translation>Par</translation>
    </message>
</context>
<context>
    <name>GeneralPreferences</name>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="-166"/>
        <source>English</source>
        <translation>Inglês</translation>
    </message>
    <message>
        <location line="-1"/>
        <source>German</source>
        <translation>Alemão</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>French</source>
        <translation>Francês</translation>
    </message>
    <message>
        <location line="-3"/>
        <source>Spanish</source>
        <translation>Espanhol</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Norvegian Bokmal</source>
        <translation>Norueguês bokmål</translation>
    </message>
    <message>
        <location line="-11"/>
        <source>Czech</source>
        <translation>Tcheco</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Dutch</source>
        <translation>Neerlandês (Holandês)</translation>
    </message>
    <message>
        <location line="-3"/>
        <source>Italian</source>
        <translation>Italiano</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Portuguese (Brazil)</source>
        <translation>Português (Brasil)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Russian</source>
        <translation>Russo</translation>
    </message>
    <message>
        <location line="-5"/>
        <source>Japanese</source>
        <translation>Japonês</translation>
    </message>
    <message>
        <location line="-11"/>
        <source>Arabic</source>
        <translation>Árabe</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Persian (Iran)</source>
        <translation>Persa (Irã)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Finnish (Finland)</source>
        <translation>Finlandês</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Hungarian</source>
        <translation>Húngaro</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Indonesian</source>
        <translation>Indonésio</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Polish</source>
        <translation>Polonês</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Swedish</source>
        <translation>Sueco</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Thai</source>
        <translation>Tailandês</translation>
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
        <translation>Vietnamita</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Chinese</source>
        <translation>Chinês</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>General</source>
        <translation>Geral</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Application-wide default font</source>
        <translation>Fonte padrão para o programa</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Monday</source>
        <translation>Segunda-feira</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sunday</source>
        <translation>Domingo</translation>
    </message>
    <message>
        <location line="+48"/>
        <source>Next check: %1</source>
        <translation>Próxima verificação: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/GeneralPreferences.ui" line="+17"/>
        <source>General settings</source>
        <translation>Configurações gerais</translation>
    </message>
    <message>
        <location line="+35"/>
        <source>System default</source>
        <translation>Padrão do sistema</translation>
    </message>
    <message>
        <location line="-27"/>
        <source>Preferred dictionary language</source>
        <translation>Linguagem de dicionário preferida</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Preferred &amp;GUI language</source>
        <translation>Língua do &amp;programa preferida</translation>
    </message>
    <message>
        <location line="+34"/>
        <source>The dictionaries will use this language whenever possible. English will be used as a fallback.</source>
        <translation>Os dicionários irão usar esta linguagem sempre que possível. O inglês será usado caso necessário.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Same as GUI</source>
        <translation>Igual ao programa.</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>You need to restart Tagaini Jisho for these changes to take effect.</source>
        <translation>Você precisa reiniciar o Tagaini Jisho para que estas mudanças tenham efeito.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Dates</source>
        <translation>Datas</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Updates</source>
        <translation>Atualizações</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Check for updates</source>
        <translation>Verificar se há atualizações</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>every day</source>
        <translation>todos os dias</translation>
    </message>
    <message>
        <location line="+3"/>
        <source> days</source>
        <translation>dias</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>every </source>
        <translation>todo</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Also check for development versions</source>
        <translation>Também verificar por versões de desenvolvimento.</translation>
    </message>
    <message>
        <location line="-113"/>
        <source>The GUI will use this language whenever possible. English will be used as a fallback.</source>
        <translation>O programa irá usar este idioma sempre que possível. O Inglês será usado quando necessário.</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>If checked, English meanings will be shown even if a translation in the preferred language is available.</source>
        <translation>Se marcado, os significados em Inglês serão mostrados mesmo que uma tradução na língua preferida esteja disponível.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Always show English meanings</source>
        <translation>Sempre mostrar significados em Inglês</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Wee&amp;ks start on</source>
        <translation>&amp;Semanas começam em</translation>
    </message>
    <message>
        <location line="+59"/>
        <source>Next check:</source>
        <translation>Próxima verificação:</translation>
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
        <translation>JLPT:</translation>
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
        <translation type="unfinished"/>
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
        <translation>Leituras alternativas:</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Alternate writings:</source>
        <translation>Escritas alternativas</translation>
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
        <translation>Frases de exemplo</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>JMdict ID</source>
        <translation>ID do JMdict</translation>
    </message>
</context>
<context>
    <name>JMdictFilterWidget</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictFilterWidget.cc" line="+42"/>
        <source>Using studied kanji only</source>
        <translation>Utilizando somente kanjis já estudados</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>With kanji:</source>
        <translation>Com kanji:</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>With components:</source>
        <translation>Com componentes:</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Include kana-only words</source>
        <translation>Incluir palavras escritas apenas com kana</translation>
    </message>
    <message>
        <location line="+13"/>
        <location line="+117"/>
        <source>Part of speech</source>
        <translation>Classe gramatical</translation>
    </message>
    <message>
        <location line="-111"/>
        <source>Dialect</source>
        <translation>Dialeto</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Field</source>
        <translation>Campo</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Misc</source>
        <translation>Miscelâneo</translation>
    </message>
    <message>
        <location line="+53"/>
        <source> with </source>
        <translation>com</translation>
    </message>
    <message>
        <location line="+9"/>
        <source> with component </source>
        <translation>com componente</translation>
    </message>
    <message>
        <location line="+9"/>
        <source> with studied kanji</source>
        <translation>com kanjis estudados</translation>
    </message>
    <message>
        <location line="+4"/>
        <source> using kana only</source>
        <translation>usando somente kana</translation>
    </message>
    <message>
        <location line="-5"/>
        <source>, studied kanji only</source>
        <translation>, somente kanjis estudados</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>, including kana words</source>
        <translation>, incluindo palavras escritas com kana</translation>
    </message>
    <message>
        <location line="+9"/>
        <location line="+1"/>
        <source>Vocabulary</source>
        <translation>Vocabulário</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Pos:</source>
        <translation>Classe:</translation>
    </message>
</context>
<context>
    <name>JMdictGUIPlugin</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictGUIPlugin.cc" line="+61"/>
        <source>Vocabulary flashcards</source>
        <translation>Cartões de memorização de vocabulário</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;japanese, whole study list</source>
        <translation>Do &amp;japonês, lista de estudos completa</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;japanese, current set</source>
        <translation>Do &amp;japonês, busca atual</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>From &amp;translation, whole study list</source>
        <translation>Da &amp;tradução, lista de estudos completa</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;translation, current set</source>
        <translation>Da &amp;tradução, busca atual</translation>
    </message>
    <message>
        <location line="+104"/>
        <location line="+19"/>
        <source>Nothing to train</source>
        <translation>Nada para treinar</translation>
    </message>
    <message>
        <location line="-19"/>
        <location line="+19"/>
        <source>There are no vocabulary entries in this set to train on.</source>
        <translation>Não há entradas de vocabulário para treinar nesta busca.</translation>
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
        <translation>matemática</translation>
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
        <translation>gíria de manga</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>polite (teineigo) language</source>
        <translation>linguagem polida (teineigo)</translation>
    </message>
    <message>
        <location line="+105"/>
        <source>su verb - precursor to the modern suru</source>
        <translation>verbo su - precursor do moderno suru</translation>
    </message>
    <message>
        <location line="-245"/>
        <source>Hokkaido-ben</source>
        <translation>Hokkaido-ben</translation>
    </message>
    <message>
        <location line="+154"/>
        <source>vulgar expression or word</source>
        <translation>expressão ou palavra vulgar</translation>
    </message>
    <message>
        <location line="-29"/>
        <source>idiomatic expression</source>
        <translation>Expressão idiomática</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>honorific or respectful (sonkeigo) language</source>
        <translation>linguagem honorífica ou respeitosa (sonkeigo)</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>female term or language</source>
        <translation>termo ou linguagem feminina</translation>
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
        <translation>leitura ateji (fonética)</translation>
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
        <translation>linguagem infantil</translation>
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
        <translation>verbo de ligação</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>expressions (phrases, clauses, etc.)</source>
        <translation>expressões (frases, orações, etc.)</translation>
    </message>
    <message>
        <location line="+73"/>
        <source>gikun (meaning as reading) or jukujikun (special kanji reading)</source>
        <translation>gikun (sentido como leitura) ou jukujikun (leitura especial de kanji)</translation>
    </message>
    <message>
        <location line="-152"/>
        <source>word containing irregular kanji usage</source>
        <translation>palavra contendo uso irregular de kanji</translation>
    </message>
    <message>
        <location line="-1"/>
        <location line="+154"/>
        <source>word containing irregular kana usage</source>
        <translation>palavra contendo uso irregular de kana</translation>
    </message>
    <message>
        <location line="-152"/>
        <source>irregular okurigana usage</source>
        <translation>uso irregular de okurigana</translation>
    </message>
    <message>
        <location line="+153"/>
        <source>out-dated or obsolete kana usage</source>
        <translation>uso de kana desatualizado ou obsoleto</translation>
    </message>
    <message>
        <location line="-118"/>
        <source>onomatopoeic or mimetic word</source>
        <translation>palavra onomatopeica ou mimética</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>poetical term</source>
        <translation>termo poético</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>proverb</source>
        <translation>provérbio</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>quotation</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+3"/>
        <source>sensitive</source>
        <translation>sensível</translation>
    </message>
    <message>
        <location line="+44"/>
        <source>unclassified</source>
        <translation>não classificado</translation>
    </message>
    <message>
        <location line="-33"/>
        <source>yojijukugo</source>
        <translation>yojijukugo</translation>
    </message>
    <message>
        <location line="+36"/>
        <source>Ichidan verb - kureru special class</source>
        <translation>Verbo ichidan - classe especial kureru</translation>
    </message>
    <message>
        <location line="+53"/>
        <source>suru verb - included</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="-82"/>
        <source>archaic/formal form of na-adjective</source>
        <translation>forma arcaica/formal de adjetivo &apos;na&apos;</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>proper noun</source>
        <translation>nome próprio</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>verb unspecified</source>
        <translation>verbo não especificado</translation>
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
        <translation>adjetivo (keiyoushi) - classe yoi/ii</translation>
    </message>
    <message>
        <location line="-34"/>
        <source>jocular, humorous term</source>
        <translation>termo jocoso, de humor</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>humble (kenjougo) language</source>
        <translation>linguagem humilde (kenjougo)</translation>
    </message>
    <message>
        <location line="-21"/>
        <source>abbreviation</source>
        <translation>abreviação</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>familiar language</source>
        <translation>linguagem familiar</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>rude or X-rated term (not displayed in educational software)</source>
        <translation>termo rude ou pornográfico (não exibido em programas educacionais)</translation>
    </message>
    <message>
        <location line="-44"/>
        <source>derogatory</source>
        <translation>derrogatório</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>word usually written using kana alone</source>
        <translation>palavra normalmente escrita usando somente kana</translation>
    </message>
    <message>
        <location line="-3"/>
        <source>slang</source>
        <translation>gíria</translation>
    </message>
    <message>
        <location line="-20"/>
        <source>male term or language</source>
        <translation>termo ou linguagem masculina</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>obsolete term</source>
        <translation>termo obsoleto</translation>
    </message>
    <message>
        <location line="+51"/>
        <source>noun (temporal) (jisoumeishi)</source>
        <translation>substantivo (temporal) (jisoumeishi)</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>prefix</source>
        <translation>prefixo</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>interjection (kandoushi)</source>
        <translation>interjeição (kandoushi)</translation>
    </message>
    <message>
        <location line="+61"/>
        <source>Godan verb - Uru old class verb (old form of Eru)</source>
        <translation>verbo Godan - Uru classe antiga de verbo (forma antiga de Eru)</translation>
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
        <translation>verbo intransitivo</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Kuru verb - special class</source>
        <translation>verbo Kuru - classe especial</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>irregular nu verb</source>
        <translation>verbo &apos;nu&apos; irregular</translation>
    </message>
    <message>
        <location line="-54"/>
        <source>particle</source>
        <translation>partícula</translation>
    </message>
    <message>
        <location line="+55"/>
        <source>irregular ru verb, plain form ends with -ri</source>
        <translation>forma ru irregular, forma do dicionário termina com -ri</translation>
    </message>
    <message>
        <location line="-85"/>
        <source>noun or verb acting prenominally</source>
        <translation>substantivo ou verbo agindo prenominalmente</translation>
    </message>
    <message>
        <location line="+90"/>
        <source>transitive verb</source>
        <translation>verbo transitivo</translation>
    </message>
    <message>
        <location line="-65"/>
        <source>noun, used as a suffix</source>
        <translation>substantivo, utilizado como sufixo</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>conjunction</source>
        <translation>conjunção</translation>
    </message>
    <message>
        <location line="-15"/>
        <source>adjective (keiyoushi)</source>
        <translation>adjetivo (keiyoushi)</translation>
    </message>
    <message>
        <location line="+90"/>
        <source>Ichidan verb - zuru verb (alternative form of -jiru verbs)</source>
        <translation>verbo Ichidan - verbo zuru (forma alternativa dos verbos -jiru)</translation>
    </message>
    <message>
        <location line="-80"/>
        <source>adverb (fukushi)</source>
        <translation>advérbio (fukushi)</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>suffix</source>
        <translation>sufixo</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>Godan verb - -aru special class</source>
        <translation>verbo Godan - classe especial -aru</translation>
    </message>
    <message>
        <location line="-55"/>
        <source>auxiliary</source>
        <translation>auxiliar</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>auxiliary verb</source>
        <translation>verbo auxiliar</translation>
    </message>
    <message>
        <location line="-10"/>
        <source>adjectival nouns or quasi-adjectives (keiyodoshi)</source>
        <translation>substantivos adjetivos ou semi-adjetivos (keiyodoshi)</translation>
    </message>
    <message>
        <location line="+67"/>
        <source>Godan verb - Iku/Yuku special class</source>
        <translation>verbo Godan - classe especial Iku-Yuku</translation>
    </message>
    <message>
        <location line="-38"/>
        <source>Ichidan verb</source>
        <translation>verbo Ichidan</translation>
    </message>
    <message>
        <location line="-10"/>
        <source>noun, used as a prefix</source>
        <translation>substantivo, usado como prefixo</translation>
    </message>
    <message>
        <location line="-10"/>
        <source>auxiliary adjective</source>
        <translation>adjetivo auxiliar</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>numeric</source>
        <translation>numérico</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>counter</source>
        <translation>contador</translation>
    </message>
    <message>
        <location line="-10"/>
        <source>pre-noun adjectival (rentaishi)</source>
        <translation>adjetival pré-substantivo (rentaishi)</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Nidan verb with &apos;u&apos; ending (archaic)</source>
        <translation>verbo Nidan com terminação &apos;u&apos; (arcaico)</translation>
    </message>
    <message>
        <location line="+53"/>
        <source>suru verb - special class</source>
        <translation>verbo suru - classe especial</translation>
    </message>
    <message>
        <location line="-68"/>
        <source>noun (common) (futsuumeishi)</source>
        <translation>substantivo (comum) (futsuumeishi)</translation>
    </message>
    <message>
        <location line="+65"/>
        <source>noun or participle which takes the aux. verb suru</source>
        <translation>substantivo ou particípio que usa o verbo aux. suru</translation>
    </message>
    <message>
        <location line="-64"/>
        <source>adverbial noun (fukushitekimeishi)</source>
        <translation>substantivo adverbial (fukushitekimeishi)</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>pronoun</source>
        <translation>pronome</translation>
    </message>
</context>
<context>
    <name>JMdictPreferences</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictPreferences.cc" line="+24"/>
        <source>Vocabulary entries</source>
        <translation>Entradas de vocabulário</translation>
    </message>
    <message>
        <location filename="../src/gui/jmdict/JMdictPreferences.ui" line="+14"/>
        <source>Form</source>
        <translation>Formulário</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Display</source>
        <translation>Exibição</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Basic information</source>
        <translation>Informação básica</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>JLPT level</source>
        <translation>Nível JLPT</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Kanji used in main reading</source>
        <translation>Kanji usado na leitura principal</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>JMdict ID (only for debugging)</source>
        <translation>ID do JMdict (somente para debug)</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Additional information</source>
        <translation>Informação adicional</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>For transitive verbs, look for the equivalent intransitive entry and vice-versa.</source>
        <translation>Para verbos transitivos, olhar a entrada intransitiva equivalente, e vice-versa</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Search for transitive/intransitive verb buddy</source>
        <translation>Procurar pelo par transitivo/intransitivo</translation>
    </message>
    <message>
        <location line="+12"/>
        <location line="+24"/>
        <source>Display words that have the same pronunciation.</source>
        <translation>Mostrar palavras com a mesma pronúncia.</translation>
    </message>
    <message>
        <location line="-21"/>
        <source>Homophones</source>
        <translation>Homófonos</translation>
    </message>
    <message>
        <location line="+7"/>
        <location line="+24"/>
        <source>Studied only</source>
        <translation>Somente estudados</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Homographs</source>
        <translation>Homógrafos</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Link to example sentences:</source>
        <translation>Link para frases de exemplo:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>If enabled, display a link that will look for example sentences in your web browser using the selected service</source>
        <translation>Se ativado, mostra um link que irá buscar frases de exemplo no seu navegador da Internet utilizando o serviço selecionado</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Disabled</source>
        <translation>Desativado</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Printing</source>
        <translation>Impressão</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>General</source>
        <translation>Geral</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Header font size:</source>
        <translation>Tamanho da fonte do cabeçalho:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Maximum number of definitions:</source>
        <translation>Número máximo de definições:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>All</source>
        <translation>Todos</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Kanji</source>
        <translation>Kanjio</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kanji used in word</source>
        <translation>Kanjis utilizados na palavra</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Only studied kanji</source>
        <translation>Somente kanjis estudados</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Preview</source>
        <translation>Visualização</translation>
    </message>
    <message>
        <location line="+142"/>
        <source>Definitions filtering</source>
        <translation>Filtragem de definições</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Filtered definitions</source>
        <translation>Definições filtradas</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Displayed:</source>
        <translation>Exibindo:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Definitions types in this list will be searched for and displayed.</source>
        <translation>Tipos de definições nesta lista serão procurados e exibidos.</translation>
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
        <translation>Filtrado:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Definitions types in this list will be ignored when searching and displaying entries.</source>
        <translation>Tipos de definição nesta lista serão ignorados ao procurar e exibir entradas.</translation>
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
        <translation>Formulário</translation>
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
        <translation>Obsoleto</translation>
    </message>
</context>
<context>
    <name>KanaView</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanaView.cc" line="+162"/>
        <source>Select All</source>
        <translation>Selecionar todos</translation>
    </message>
</context>
<context>
    <name>KanjiComponentWidget</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPopup.cc" line="+100"/>
        <source>%1 (drawn as &quot;%2&quot;)</source>
        <translation>%1 (escrito como &quot;%2&quot;)</translation>
    </message>
</context>
<context>
    <name>KanjiInputPopupAction</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiSelector.cc" line="+398"/>
        <source>Triggers the kanji input panel</source>
        <translation>Ativa o painel de inserção de kanji</translation>
    </message>
</context>
<context>
    <name>KanjiPlayer</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPlayer.cc" line="+50"/>
        <source>Play</source>
        <translation>Reproduzir</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pause</source>
        <translation>Pausar</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Stop</source>
        <translation>Parar</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Next stroke</source>
        <translation>Próximo traço</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Previous stroke</source>
        <translation>Traço anterior</translation>
    </message>
</context>
<context>
    <name>KanjiPopup</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPopup.ui" line="+81"/>
        <source>See in detailed view</source>
        <translation>Ver na visão detalhada</translation>
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
        <translation>&lt;b&gt;Freq:&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>&lt;b&gt;Grade:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;Ano escolar:&lt;/b&gt; %1&lt;br/&gt;</translation>
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
        <translation>&lt;b&gt;Radicais:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="-28"/>
        <source>&lt;b&gt;Score:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;Pontuação:&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPopup.ui" line="-61"/>
        <source>Form</source>
        <translation>Formulário</translation>
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
        <translation>Próxima entrada</translation>
    </message>
    <message>
        <location line="+50"/>
        <source>Entry options menu</source>
        <translation>Opções do menu de entrada</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Close this popup</source>
        <translation>Fechar esta janela</translation>
    </message>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPopup.cc" line="-36"/>
        <source>No information about this kanji!</source>
        <translation>Nenhuma informação sobre este kanji!</translation>
    </message>
</context>
<context>
    <name>KanjiSelector</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiSelector.ui" line="+6"/>
        <source>Radical search</source>
        <translation>Busca por radical</translation>
    </message>
</context>
<context>
    <name>Kanjidic2Entry</name>
    <message>
        <location filename="../src/core/kanjidic2/Kanjidic2Entry.cc" line="+126"/>
        <source>(var) </source>
        <translation>(var) </translation>
    </message>
</context>
<context>
    <name>Kanjidic2EntryFormatter</name>
    <message>
        <location filename="../src/gui/kanjidic2/Kanjidic2EntryFormatter.cc" line="+338"/>
        <source>&lt;b&gt;Strokes:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;Traços:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>&lt;b&gt;Frequency:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;Frequência:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>&lt;b&gt;Grade:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;Ano escolar:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>&lt;b&gt;4 corner:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;4 cantos:&lt;/b&gt; %1</translation>
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
        <translation>Traços</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Frequency</source>
        <translation>Frequência</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Grade</source>
        <translation>Ano escolar</translation>
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
        <translation>Variações</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Variation of</source>
        <translation>Variação de</translation>
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
        <translation>4 cantos</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Radicals</source>
        <translation>Radicais</translation>
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
        <translation>Contagem de traços</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Range</source>
        <translation>Alcance</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Radicals</source>
        <translation>Radicais</translation>
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
        <translation>Quatro cantos</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>School grade</source>
        <translation>Ano escolar</translation>
    </message>
    <message>
        <location line="+3"/>
        <location line="+261"/>
        <source>Grade</source>
        <translation>Ano escolar</translation>
    </message>
    <message>
        <location line="-247"/>
        <source>All &amp;Kyouiku kanji</source>
        <translation>Todos os kanji &amp;Kyouiku</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>All &amp;Jouyou kanji</source>
        <translation>Todos os kanji &amp;Jouyou</translation>
    </message>
    <message>
        <location line="+219"/>
        <source>, %1 strokes</source>
        <translation>, %1 traço</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>, strokes&lt;=%1</source>
        <translation>, traços&lt;=%1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>, strokes&gt;=%1</source>
        <translation>, traços&gt;=%1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>, %1-%2 strokes</source>
        <translation>, %1-%2 traços</translation>
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
        <translation>, 4c: %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>, grade: %1</source>
        <translation>, ano escolar: %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <location line="+1"/>
        <source>Kanji</source>
        <translation>Kanjio</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Grade:</source>
        <translation>Ano escolar:</translation>
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
        <translation>1ª série do fundamental</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 2nd grade</source>
        <translation>2ª série do fundamental</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 3rd grade</source>
        <translation>3ª série do fundamental</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 4th grade</source>
        <translation>4ª série do fundamental</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 5th grade</source>
        <translation>5ª série do fundamental</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 6th grade</source>
        <translation>6ª série do fundamental</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Junior high school</source>
        <translation>Colegial</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Used for names (1)</source>
        <translation>Usado para nomes (1)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Used for names (2)</source>
        <translation>Usado para nomes (2)</translation>
    </message>
    <message>
        <location line="+33"/>
        <source>Character flashcards</source>
        <translation>Cartões de memorização de caracteres</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;character, whole study list</source>
        <translation>Do &amp;caractere, toda a lista de estudos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;character, current set</source>
        <translation>Do &amp;caractere, busca atual</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>From &amp;meaning, whole study list</source>
        <translation>Do &amp;significado, toda a lista de estudos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;meaning, current set</source>
        <translation>Do &amp;significado, busca atual</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>&amp;Reading practice, whole study list</source>
        <translation>%Prática de leitura, lista de estudos completa</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kana</source>
        <translation>Kana</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Radical search input</source>
        <translation>Entrada de busca por radicais</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Component search input</source>
        <translation>Entrada de busca por componentes</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Show stroke popup for currently displayed kanji</source>
        <translation>Mostrar janela de traços para o kanji atualmente exibido</translation>
    </message>
    <message>
        <location line="+94"/>
        <location line="+18"/>
        <source>Nothing to train</source>
        <translation>Nada para treinar</translation>
    </message>
    <message>
        <location line="-18"/>
        <location line="+18"/>
        <source>There are no kanji entries in this set to train on.</source>
        <translation>Não há entradas de kanji neste conjunto para treinar</translation>
    </message>
    <message>
        <location line="+186"/>
        <source>Open in detailed view...</source>
        <translation>Abrir em visão detalhada...</translation>
    </message>
    <message>
        <location line="+25"/>
        <source>Tracing for %1</source>
        <translation>Tracejamento para %1</translation>
    </message>
</context>
<context>
    <name>Kanjidic2Preferences</name>
    <message>
        <location filename="../src/gui/kanjidic2/Kanjidic2Preferences.ui" line="+14"/>
        <source>Form</source>
        <translation>Formulário</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Display</source>
        <translation>Exibição</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Basic information</source>
        <translation>Informação básica</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Readings</source>
        <translation>Leituras</translation>
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
        <translation>Variações</translation>
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
        <translation>Frequência</translation>
    </message>
    <message>
        <location line="-469"/>
        <location line="+476"/>
        <source>School grade</source>
        <translation>Ano escolar</translation>
    </message>
    <message>
        <location line="-483"/>
        <location line="+490"/>
        <source>JLPT level</source>
        <translation>Nível JLPT</translation>
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
        <translation>número de traços</translation>
    </message>
    <message>
        <location line="-518"/>
        <location line="+504"/>
        <source>SKIP code</source>
        <translation>Código SKIP</translation>
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
        <translation>Código de quatro cantos</translation>
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
        <translation>Informação adicional</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Radicals</source>
        <translation>Radicais</translation>
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
        <translation>Somente estudados</translation>
    </message>
    <message>
        <location line="-14"/>
        <source>Words using the kanji:</source>
        <translation>Palavras que utilizam este kanji:</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>Printing</source>
        <translation>Impressão</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kanji information</source>
        <translation>Informação do Kanji</translation>
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
        <translation>Somente componentes estudados</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>words</source>
        <translation>palavras</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Only studied words</source>
        <translation>Somente palavras estudadas</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Kanji drawing style</source>
        <translation>Estilo de escrita do kanji</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Handwriting</source>
        <translation>Escrita à mão</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Kanji header size:</source>
        <translation>Tamanho do cabeçalho do kanji:</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Print grid</source>
        <translation>Imprimir grade</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Show stroke numbers</source>
        <translation>Mostrar números dos traços</translation>
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
        <translation>Visualização</translation>
    </message>
    <message>
        <location line="-322"/>
        <source>Tooltip</source>
        <translation>Janela</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Score</source>
        <translation>Pontuação</translation>
    </message>
    <message>
        <location line="+73"/>
        <source>Stroke animation</source>
        <translation>Animação dos traços</translation>
    </message>
    <message>
        <location line="+181"/>
        <source> seconds</source>
        <translation>segundos</translation>
    </message>
    <message>
        <location line="+32"/>
        <source>Kana selector</source>
        <translation>Seletor de kana</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Appearance</source>
        <translation>Aparência</translation>
    </message>
    <message>
        <location line="-484"/>
        <location line="+280"/>
        <source>Size:</source>
        <translation>Tamanho:</translation>
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
        <translation>Velocidade da escrita:</translation>
    </message>
    <message>
        <location line="+9"/>
        <location line="+31"/>
        <source>Default</source>
        <translation>Padrão</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Pause between strokes:</source>
        <translation>Pausa entre traços:</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Show numbers</source>
        <translation>Mostrar números</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Show grid</source>
        <translation>Mostrar grade</translation>
    </message>
    <message>
        <location line="+39"/>
        <source>Animation playback</source>
        <translation>Execução da animação</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Play animation when the kanji popup appears</source>
        <translation>Mostrar animação quando a janela de kanji aparece</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Loop animation after:</source>
        <translation>Repetir animação após:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Do not loop</source>
        <translation>Não repetir</translation>
    </message>
    <message>
        <location filename="../src/gui/kanjidic2/Kanjidic2Preferences.cc" line="+27"/>
        <source>Character entries</source>
        <translation>Entradas de caracteres</translation>
    </message>
    <message>
        <location line="+43"/>
        <source>Character font</source>
        <translation>Fonte de caracteres</translation>
    </message>
</context>
<context>
    <name>ListsViewPreferences</name>
    <message>
        <location filename="../src/gui/ListsViewPreferences.ui" line="+17"/>
        <source>Lists display</source>
        <translation>Exibição de listas</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Smooth scrolling</source>
        <translation>Rolagem suave</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Preview</source>
        <translation>Visualização</translation>
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
        <translation>&amp;Sair</translation>
    </message>
    <message>
        <location line="-114"/>
        <source>&amp;Program</source>
        <translation>&amp;Programa</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>&amp;Search</source>
        <translation>&amp;Busca</translation>
    </message>
    <message>
        <location line="-11"/>
        <source>&amp;Help</source>
        <translation>&amp;Ajuda</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>P&amp;ractice</source>
        <translation>Prát&amp;ica</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>S&amp;aved searches</source>
        <translation>&amp;Buscas salvas</translation>
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
        <translation>S&amp;obre</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Focus results list</source>
        <translation>Focar na lista de resultados</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Ctrl+O</source>
        <translation>Ctrl+O</translation>
    </message>
    <message>
        <location line="-115"/>
        <source>Preferences...</source>
        <translation>Preferências...</translation>
    </message>
    <message>
        <location line="+56"/>
        <source>&amp;Report a bug...</source>
        <translation>&amp;Informar um problema...</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>&amp;Suggest a feature...</source>
        <translation>&amp;Sugerir um recurso...</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Ask a &amp;question...</source>
        <translation>Fazer uma &amp;pergunta...</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Support Tagaini Jisho - Make a &amp;donation!</source>
        <translation>Ajude o Tagaini Jisho - Fazer uma &amp;doação!</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Focus text search</source>
        <translation>Focar na busca por texto</translation>
    </message>
    <message>
        <location line="-122"/>
        <location filename="../src/gui/MainWindow.cc" line="+235"/>
        <location line="+7"/>
        <source>Export user data...</source>
        <translation>Exportar dados do usuário...</translation>
    </message>
    <message>
        <location line="+9"/>
        <location filename="../src/gui/MainWindow.cc" line="+6"/>
        <source>Import user data...</source>
        <translation>Importar dados do usuário...</translation>
    </message>
    <message>
        <location line="-123"/>
        <source>Tagaini Jisho</source>
        <translation>Tagaini Jisho</translation>
    </message>
    <message>
        <location line="+80"/>
        <source>Search</source>
        <translation>Busca</translation>
    </message>
    <message>
        <location line="+75"/>
        <source>&amp;Save current search...</source>
        <translation>&amp;Salvar busca atual...</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>&amp;New folder...</source>
        <translation>&amp;Nova pasta...</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>&amp;Organize saved searches...</source>
        <translation>&amp;Organizar buscas salvas...</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Whole study list train &amp;settings...</source>
        <translation>&amp;Preferências de treino da lista de estudos completa</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cc" line="-55"/>
        <source>Auto-search on clipboard content</source>
        <translation>Busca automática do conteúdo na área de transferência</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Please consider supporting Tagaini Jisho</source>
        <translation>Por favor, leve em conta a possibilidade de ajudar o Tagaini Jisho</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You have now been using Tagaini Jisho for a short while. If you like it, please show your support by making a donation that expresses your appreciation of this software. While Tagaini is free, its development has a cost and your support will have a positive influence on its future. You can donate via Paypal or credit card.

This message is a one-time reminder and will not appear anymore - you can still donate later by choosing the corresponding option in the Help menu.</source>
        <translation>Você tem usado o Tagaini Jisho por algum tempo. Se você gostou, por favor mostre seu apoio fazendo uma pequena doação que expressa sua apreciação por este programa. Embora o Tagaini seja gratuito, o seu desenvolvimento possui um custo, e o seu apoio terá uma influência positiva no seu futuro. Você pode doar via Paypal ou cartão de crédito.

Esta mensagem é um lembrete que aparecerá uma única vez e não aparecerá mais - você ainda pode doar mais tarde ao escolher a opção correspondente no menu de Ajuda.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Donate!</source>
        <translation>Fazer doação!</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Nah</source>
        <translation>Nhe</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Error: Could not export database! Please verify that you have write permissions on the target file.</source>
        <translation>Erro: Impossível exportar banco de dados! Por favor verifique se você possui permissões para alterar o arquivo selecionado.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>About to replace user data!</source>
        <translation>Pronto para substituir dados do usuário!</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>&lt;b&gt;WARNING:&lt;/b&gt; All the user data (study list, tags, notes, training, ...) is going to be replaced by the selected user data file. Are you &lt;b&gt;sure&lt;/b&gt; you want to do this?</source>
        <translation>&lt;b&gt;AVISO:&lt;/b&gt; Todos os dados do usuário (lista de estudos, etiquetas, anotações, treinamento, ...) serão substituídos pelo arquivo escolhido. Você tem &lt;b&gt;certeza&lt;/b&gt; que deseja fazer isso?</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Please restart Tagaini Jisho</source>
        <translation>Por favor reinicie o Tagaini Jisho</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The imported data will be available the next time you start Tagaini Jisho. All changes performed from now will be lost. Do you want to exit Tagaini Jisho now?</source>
        <translation>Os dados importados estarão disponíveis na próxima vez que você iniciar o Tagaini Jisho. Todas as mudanças feitas de agora em diante serão perdidas. Você deseja fechar o Tagaini Jisho agora?</translation>
    </message>
    <message>
        <location line="+113"/>
        <source>A development update is available!</source>
        <translation>Uma atualização de desenvolvimento está disponível!</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Development version %1 of Tagaini Jisho is available. Do you want to download it now?</source>
        <translation>Versão de desenvolvimento %1 do Tagaini Jisho está disponível. Você deseja baixar agora?</translation>
    </message>
    <message>
        <location line="+54"/>
        <source>Save current search here...</source>
        <translation>Salvar busca atual aqui...</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Save current search</source>
        <translation>Salvar busca atual</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Please enter a name for this search:</source>
        <translation>Por favor coloque um nome para esta pequisa:</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unnamed</source>
        <translation>Sem nome</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>New folder</source>
        <translation>Nova pasta</translation>
    </message>
    <message>
        <location line="-34"/>
        <source>Create new folder here...</source>
        <translation>Criar nova pasta aqui...</translation>
    </message>
    <message>
        <location line="+34"/>
        <source>Please enter a name for this folder</source>
        <translation>Por favor coloque um nome para esta pasta</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unnamed folder</source>
        <translation>Pasta sem nome</translation>
    </message>
    <message>
        <location line="-106"/>
        <source>An update is available!</source>
        <translation>Uma atualização está disponível!</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Version %1 of Tagaini Jisho is available. Do you want to download it now?</source>
        <translation>Versão %1 do Tagaini Jisho está disponível. Você deseja baixar agora?</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+15"/>
        <source>Let&apos;s go!</source>
        <translation>Vamos lá!</translation>
    </message>
    <message>
        <location line="-13"/>
        <location line="+15"/>
        <source>Maybe later</source>
        <translation>Talvez mais tarde</translation>
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
        <translation>Limpar este filtro</translation>
    </message>
</context>
<context>
    <name>NotesFilterWidget</name>
    <message>
        <location filename="../src/gui/NotesFilterWidget.cc" line="+50"/>
        <source>Notes</source>
        <translation>Anotações</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Has note</source>
        <translation>Tem anotação</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Note contains %1</source>
        <translation>Anotação contém %1</translation>
    </message>
</context>
<context>
    <name>PreferencesFontChooser</name>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="+156"/>
        <source>Change...</source>
        <translation>Alterar...</translation>
    </message>
    <message>
        <location line="-5"/>
        <source>%1:</source>
        <translation>%1:</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Default</source>
        <translation>Padrão</translation>
    </message>
</context>
<context>
    <name>PreferencesWindow</name>
    <message>
        <location filename="../src/gui/PreferencesWindow.ui" line="+14"/>
        <source>Preferences</source>
        <translation>Preferências</translation>
    </message>
</context>
<context>
    <name>RadicalSearchWidget</name>
    <message>
        <location filename="../src/gui/kanjidic2/RadicalSearchWidget.ui" line="+14"/>
        <source>Radical search</source>
        <translation>Busca por radical</translation>
    </message>
</context>
<context>
    <name>ReadingTrainer</name>
    <message>
        <location filename="../src/gui/ReadingTrainer.ui" line="+14"/>
        <location filename="../src/gui/ReadingTrainer.cc" line="+41"/>
        <source>Reading practice</source>
        <translation>Prática de leitura</translation>
    </message>
    <message>
        <location filename="../src/gui/ReadingTrainer.cc" line="+5"/>
        <source>Show &amp;meaning</source>
        <translation>Mostrar &amp;significado</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Preparing training session</source>
        <translation>Preparando sessão de treino</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Preparing training session, please wait...</source>
        <translation>Preparando sessão de treino, por favor aguarde...</translation>
    </message>
    <message>
        <location line="+57"/>
        <source>No matching entries found</source>
        <translation>Nenhuma entrada correspondente encontrada</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unable to find any entry eligible for reading practice. Entries eligible for this training mode are studied vocabulary entries for which all kanji are also studied, and that match the train settings. Please add entries or modify the train settings accordingly if you want to practice this mode.</source>
        <translation>Incapaz de encontrar qualquer entrada apropriada para a prática de leitura. As entradas apropriadas para este modo de treino são as entradas já estudadas de vocabulário nas quais todos os kanjis também já foram estudados, além de também estarem de acordo com as opções do treino. Por favor adicione entradas ou modifique as configurações de treino para poder praticar este modo.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>There are no more entries to train for the current train settings.</source>
        <translation>Não há mais nenhuma entrada para treinar para as atuais configurações de treino.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No more entries to train</source>
        <translation>Sem mais entradas para treinar</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>&lt;font color=&quot;green&quot;&gt;Correct!&lt;/font&gt;</source>
        <translation>&lt;font color=&quot;green&quot;&gt;Correto!&lt;/font&gt;</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>&lt;font color=&quot;red&quot;&gt;Error!&lt;/font&gt;</source>
        <translation>&lt;font color=&quot;red&quot;&gt;Erro!&lt;/font&gt;</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>Correct: %1, Wrong: %2, Total: %3</source>
        <translation>Correto: %1, Errado: %2, Total: %3</translation>
    </message>
    <message>
        <location filename="../src/gui/ReadingTrainer.ui" line="+9"/>
        <source>Type the correct reading for this word:</source>
        <translation>Digite a leitura correta para esta palavra:</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>&amp;Next</source>
        <translation>&amp;Próximo</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>&amp;Ok!</source>
        <translation>&amp;Ok!</translation>
    </message>
</context>
<context>
    <name>RelativeDate</name>
    <message>
        <location filename="../src/core/RelativeDate.cc" line="+99"/>
        <source>today</source>
        <translation>hoje</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>yesterday</source>
        <translation>ontem</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 days ago</source>
        <translation>%1 dias atrás</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>this week</source>
        <translation>esta semana</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>last week</source>
        <translation>semana passada</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 weeks ago</source>
        <translation>%1 semanas atrás</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>this month</source>
        <translation>este mês</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>last month</source>
        <translation>mês passado</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 months ago</source>
        <translation>%1 meses atrás</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>this year</source>
        <translation>este ano</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>last year</source>
        <translation>ano passado</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 years ago</source>
        <translation>%1 anos atrás</translation>
    </message>
</context>
<context>
    <name>RelativeDateEdit</name>
    <message>
        <location filename="../src/gui/RelativeDateEdit.cc" line="+163"/>
        <source>Not set</source>
        <translation>Não configurado</translation>
    </message>
</context>
<context>
    <name>RelativeDatePopup</name>
    <message>
        <location line="-130"/>
        <source>Not set</source>
        <translation>Não configurado</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>days ago</source>
        <translation>dias atrás</translation>
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
        <translation>anos atrás</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ok</source>
        <translation>Ok</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Relative date:</source>
        <translation>Data relativa:</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Absolute date:</source>
        <translation>Data absoluta:</translation>
    </message>
</context>
<context>
    <name>ResultsView</name>
    <message>
        <location filename="../src/gui/ResultsView.cc" line="+50"/>
        <source>Select All</source>
        <translation>Selecionar todos</translation>
    </message>
</context>
<context>
    <name>ResultsViewPreferences</name>
    <message>
        <location filename="../src/gui/ResultsViewPreferences.ui" line="+17"/>
        <source>Results display</source>
        <translation>Exibição de resultados</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Smooth scrolling</source>
        <translation>Rolagem suave</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Results sorting:</source>
        <translation>Ordem dos resultados:</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>By relevance</source>
        <translation>Por relevância</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>By score, then relevance</source>
        <translation>Por pontuação, depois relevância</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Preview</source>
        <translation>Visualização</translation>
    </message>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="-187"/>
        <source>Results view</source>
        <translation>Área de resultados</translation>
    </message>
</context>
<context>
    <name>ResultsViewWidget</name>
    <message>
        <location filename="../src/gui/ResultsViewWidget.cc" line="+43"/>
        <source>Searching...</source>
        <translation>Procurando...</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>%1 Results</source>
        <translation>%1 Resultados</translation>
    </message>
</context>
<context>
    <name>SavedSearchesOrganizer</name>
    <message>
        <location filename="../src/gui/SavedSearchesOrganizer.ui" line="+14"/>
        <source>Organize saved searches</source>
        <translation>Organizar buscas salvas</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>You can organize your saved searches here. Double-click to change the search&apos;s label, drag and drop to move or right-click for other options, including deletion.</source>
        <translation>Você pode organizar suas buscas salvas aqui. Dê clique duplo para mudar o nome da busca, clique e arraste para mover ou clique com o botão direito para outras opções, incluindo remoção.</translation>
    </message>
</context>
<context>
    <name>SavedSearchesTreeWidget</name>
    <message>
        <location filename="../src/gui/SavedSearchesOrganizer.cc" line="+145"/>
        <source>Delete</source>
        <translation>Excluir</translation>
    </message>
    <message>
        <location line="+36"/>
        <source>Confirm deletion</source>
        <translation>Confirmar exclusão</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Are you sure you want to delete the selected search/folder?</source>
        <translation>Tem certeza que deseja remover a busca/pasta selecionada?</translation>
    </message>
</context>
<context>
    <name>SearchWidget</name>
    <message>
        <location filename="../src/gui/SearchWidget.ui" line="+67"/>
        <source>Previous search</source>
        <translation>Busca anterior</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Next search</source>
        <translation>Próxima busca</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>&amp;Reset search...</source>
        <translation>&amp;Limpar busca...</translation>
    </message>
</context>
<context>
    <name>ShowUsedInJob</name>
    <message>
        <location filename="../src/gui/kanjidic2/Kanjidic2EntryFormatter.cc" line="+323"/>
        <source>Direct compounds</source>
        <translation>Componentes diretos</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>All compounds</source>
        <translation>Todos os componentes</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>(+)</source>
        <translation>(+)</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Make a new search using only this filter</source>
        <translation>Fazer uma nova busca usando somente este filtro</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Add this filter to the current search</source>
        <translation>Incluir este filtro na busca atual</translation>
    </message>
</context>
<context>
    <name>ShowUsedInWordsJob</name>
    <message>
        <location line="+10"/>
        <source>Seen in</source>
        <translation>Visto em</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>All words using this kanji</source>
        <translation>Todas as palavras que usam este kanji</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>(+)</source>
        <translation>(+)</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Make a new search using only this filter</source>
        <translation>Fazer uma nova busca usando somente este filtro</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Add this filter to the current search</source>
        <translation>Incluir este filtro na busca atual</translation>
    </message>
</context>
<context>
    <name>StudyFilterWidget</name>
    <message>
        <location filename="../src/gui/StudyFilterWidget.cc" line="+30"/>
        <source>Score</source>
        <translation>Pontuação</translation>
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
        <translation>Data de estudo</translation>
    </message>
    <message>
        <location line="+24"/>
        <location line="+30"/>
        <location line="+20"/>
        <source>since</source>
        <translation>desde</translation>
    </message>
    <message>
        <location line="-49"/>
        <location line="+30"/>
        <location line="+20"/>
        <source>until</source>
        <translation>até</translation>
    </message>
    <message>
        <location line="-45"/>
        <source>Last trained</source>
        <translation>Treinado pela última vez</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Last mistaken</source>
        <translation>Errado pela última vez</translation>
    </message>
    <message>
        <location line="+114"/>
        <source> since %1</source>
        <translation>desde %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <location line="+13"/>
        <location line="+10"/>
        <source> until %1</source>
        <translation>até %1</translation>
    </message>
    <message>
        <location line="-20"/>
        <source>, score:[%1-%2]</source>
        <translation>, pontuação:[%1-%2]</translation>
    </message>
    <message>
        <location line="-98"/>
        <source>Any</source>
        <translation>Qualquer</translation>
    </message>
    <message>
        <location line="+3"/>
        <location line="+85"/>
        <source>Studied</source>
        <translation>Estudado</translation>
    </message>
    <message>
        <location line="-83"/>
        <location line="+116"/>
        <source>Not studied</source>
        <translation>Não estudado</translation>
    </message>
    <message>
        <location line="-112"/>
        <source>Study status:</source>
        <translation>Status do estudo:</translation>
    </message>
    <message>
        <location line="+94"/>
        <source>, trained since %1</source>
        <translation>, treinado desde %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>, trained until %1</source>
        <translation>, treinado até %1</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>, mistaken since %1</source>
        <translation>, errado desde %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>, mistaken until %1</source>
        <translation>, errado até %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Study</source>
        <translation>Estudar</translation>
    </message>
</context>
<context>
    <name>TagsDialogs</name>
    <message>
        <location filename="../src/gui/TagsDialogs.cc" line="+175"/>
        <source>Set tags for %1</source>
        <translation>Editar etiquetas para %1</translation>
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
        <translation>Coloque uma lista de etiquetas separada por espaços para</translation>
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
        <translation>&lt;/p&gt;&lt;p&gt;Caracteres aceitos são letras, dígitos e pontuação sem aspas.&lt;/p&gt;</translation>
    </message>
    <message>
        <location line="-35"/>
        <location line="+35"/>
        <source>&lt;p&gt;These tags have invalid characters: </source>
        <translation>&lt;p&gt;Estas etiquetas têm caracteres inválidos:</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Add tags to %1</source>
        <translation>Adicionar etiquetas para %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Add tags</source>
        <translation>Adicionar etiquetas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Enter a space-separated list of tags to add to </source>
        <translation>Coloque uma lista de etiquetas separada por espaços para adicionar a</translation>
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
        <translation>Sem etiquetas</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Tags</source>
        <translation>Etiquetas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Tagged %1</source>
        <translation>Com etiquetas %1</translation>
    </message>
</context>
<context>
    <name>TextFilterWidget</name>
    <message>
        <location filename="../src/gui/TextFilterWidget.cc" line="+42"/>
        <source>Romaji search</source>
        <translation>Busca por romaji</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>If checked, text typed in romaji will be converted to kana and searched as such.</source>
        <translation>Se marcado, o texto digitado em romaji será convertido para kana e então buscado.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Text search</source>
        <translation>Busca de texto</translation>
    </message>
</context>
<context>
    <name>TrainSettings</name>
    <message>
        <location filename="../src/gui/TrainSettings.cc" line="+66"/>
        <source>Entries appear totally randomly.</source>
        <translation>Entradas aparecem totalmente aleatórias.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Entries with a low score are likely to appear first.</source>
        <translation>Entradas com pontuação baixa têm mais chances de aparecer primeiro.</translation>
    </message>
    <message>
        <location filename="../src/gui/TrainSettings.ui" line="+14"/>
        <source>Train settings</source>
        <translation>Configurações de treino</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Date of last training</source>
        <translation>Data do último treinamento</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Before</source>
        <translation>Antes</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>After</source>
        <translation>Depois</translation>
    </message>
    <message>
        <location line="+10"/>
        <location line="+7"/>
        <source>PushButton</source>
        <translation>ApertaBotão</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Score</source>
        <translation>Pontuação</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>between</source>
        <translation>entre</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>and</source>
        <translation>e</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Sort bias</source>
        <translation>Critério de ordenação</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Bias:</source>
        <translation>Critério:</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>No bias</source>
        <translation>Sem critério</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>By score</source>
        <translation>Por pontuação</translation>
    </message>
</context>
<context>
    <name>YesNoTrainer</name>
    <message>
        <location filename="../src/gui/YesNoTrainer.cc" line="+44"/>
        <source>Answer (&amp;1)</source>
        <translation>Resposta (&amp;1)</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Correct! (&amp;2)</source>
        <translation>Correto! (&amp;2)</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Wrong... (&amp;3)</source>
        <translation>Errado... (&amp;3)</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Skip (&amp;4)</source>
        <translation>Pular (&amp;4)</translation>
    </message>
    <message>
        <location line="+108"/>
        <source>No entries to train</source>
        <translation>Sem entradas para treinar</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>There are no entries to train as of now. Please add entries to your study list or check the train settings.</source>
        <translation>Não há mais entradas para treinar atualmente. Por favor adicione entradas para a sua lista de estudo ou verifique as configurações de treino.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>All the matching entries have been trained.</source>
        <translation>Todas as entradas correspondentes foram treinadas.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No more entries to train</source>
        <translation>Sem mais entradas para treinar</translation>
    </message>
    <message>
        <location line="-47"/>
        <source>Correct: %1, Wrong: %2, Total: %3</source>
        <translation>Correto: %1, Errado: %2, Total: %3</translation>
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