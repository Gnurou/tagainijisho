<?xml version="1.0" ?><!DOCTYPE TS><TS version="2.1" language="sv" sourcelanguage="en">
<context>
    <name>AboutDialog</name>
    <message>
        <location filename="../src/gui/AboutDialog.ui" line="+14"/>
        <source>About Tagaini Jisho</source>
        <translation>Om Tagaini jisho</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Tagaini Jisho</source>
        <translation>Tagaini jisho</translation>
    </message>
    <message>
        <location line="+29"/>
        <source>About</source>
        <translation>Om</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Authors &amp;&amp; contributors</source>
        <translation>Författare &amp;&amp; medarbetare</translation>
    </message>
</context>
<context>
    <name>BatchHandler</name>
    <message>
        <location filename="../src/gui/BatchHandler.cc" line="+26"/>
        <source>Marking entries...</source>
        <translation>Markerar uppslag…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Abort</source>
        <translation>Avbryt</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Please wait...</source>
        <translation>Var god vänta…</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Cannot start transaction</source>
        <translation>Kan inte inleda transaktion</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Error while trying to start database transaction.</source>
        <translation>Fel vid försök att inleda databastransaktion.</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Cannot commit transaction</source>
        <translation>Kan inte avsluta transaktion</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Error while trying to commit database transaction.</source>
        <translation>Fel vid försök att avsluta databastransaktion.</translation>
    </message>
</context>
<context>
    <name>DataPreferences</name>
    <message>
        <location filename="../src/gui/DataPreferences.ui" line="+17"/>
        <source>Erase user data</source>
        <translation>Radera användardata</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Pushing this button will completely erase all your user data. This includes study list, tags, notes, scores, basically everything you did. After pushing this button, Tagaini Jisho will exit. Upon restart, you will be back to a fresh, blank database.</source>
        <translation>Tryck på den här knappen för att radera alla dina användardata. Detta inkluderar studielistan, taggar, anteckningar, poäng; med andra ord allt du gjort. Efter att du tryckt på denna knapp kommer Tagaini jisho att avslutas. När du startar programmet nästa gång kommer Tagaini att använda en ny, tom databas.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Reset user data</source>
        <translation>Återställ användardata</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>User database file:</source>
        <translation>Fil för användardata:</translation>
    </message>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="+497"/>
        <source>User data</source>
        <translation>Användardata</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Really erase user data?</source>
        <translation>Radera användardata?</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>This will erase all your user data. Are you sure you want to do this?</source>
        <translation>Det här kommer att radera alla användardata. Vill du fortsätta?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>REALLY erase it?</source>
        <translation>Vill du VERKLIGEN radera dessa data?</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Are you absolutely, positively, definitely sure?</source>
        <translation>Är du verkligen absolut helt säker?</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>No user database file</source>
        <translation>Ingen användardatabasfil</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The user database file has already been erased before.</source>
        <translation>Den här användardatabasfilen har redan raderats.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Cannot erase user database</source>
        <translation>Kan inte radera användardatabasen</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unable to erase user database. Please see what is wrong and try to do it manually.</source>
        <translation>Kan inte radera användardatabasen. Se efter vad som är felet och försök att göra det manuellt.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>User data erased</source>
        <translation>Användardata raderade</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>User data has been erased. Tagaini Jisho will now exit. You will be back to a blank database when it is restarted.</source>
        <translation>Användardata har raderats. Tagaini jisho kommer nu att avslutas. När programmet startas nästa gång kommer en tom databas att användas.</translation>
    </message>
</context>
<context>
    <name>Database</name>
    <message>
        <location filename="../src/core/Database.cc" line="+316"/>
        <source>Error while upgrading user database: %1</source>
        <translation>Fel vid uppgradering av användardatabasen: %1</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Wrong user database version: expected %1, got %2.</source>
        <translation>Fel användardatabasversion upptäckt: förväntade %1, men fann %2.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Cannot create user database: %1</source>
        <translation>Kan inte skapa användardatabas: %1</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Cannot open database: %1</source>
        <translation>Kan inte öppna användardatabas: %1</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Temporary database fallback failed. The program will now exit.</source>
        <translation>Tillfällig databaslösning misslyckades. Programmet kommer nu att avslutas.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Tagaini is working on a temporary database. This allows the program to work, but user data is unavailable and any change will be lost upon program exit. If you corrupted your database file, please recreate it from the preferences.</source>
        <translation>Tagaini används med en tillfällig databas. Detta gör att programmet kan köras, men användardata är otillgängliga och alla ändringar kommer att förloras när programmet avslutas. Om din databasfil är korrupt, försök att återskapa den från inställningarna.</translation>
    </message>
</context>
<context>
    <name>DetailedView</name>
    <message>
        <location filename="../src/gui/DetailedView.cc" line="+101"/>
        <source>Previous entry</source>
        <translation>Föregående uppslag</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Next entry</source>
        <translation>Nästa uppslag</translation>
    </message>
</context>
<context>
    <name>DetailedViewPreferences</name>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="-108"/>
        <source>Detailed view</source>
        <translation>Detaljerad visning</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Default text</source>
        <translation>Standardtext</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kana header</source>
        <translation>Kana-rubrik</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kanji header</source>
        <translation>Kanji-rubrik</translation>
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
        <translation>Allmänt</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Smooth scrolling</source>
        <translation>Mjuk rullning</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Show JLPT level in short descriptions</source>
        <translation>Visa JLPT-nivå i den korta beskrivningen</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Fonts</source>
        <translation>Typsnitt</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Preview</source>
        <translation>Förhandsgranskning</translation>
    </message>
</context>
<context>
    <name>EditEntryNotesDialog</name>
    <message>
        <location filename="../src/gui/EditEntryNotesDialog.cc" line="+86"/>
        <source>Notes for %1</source>
        <translation>Anteckningar för %1</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Existing notes:</source>
        <translation>Existerande anteckningar:</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>&amp;New note</source>
        <translation>&amp;Ny anteckning</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>&amp;Delete note</source>
        <translation>&amp;Ta bort anteckning</translation>
    </message>
</context>
<context>
    <name>EntriesPrinter</name>
    <message>
        <location filename="../src/gui/EntriesPrinter.cc" line="+66"/>
        <source>Preparing print job...</source>
        <translation>Förbereder utskrift…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Abort</source>
        <translation>Avbryt</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Printing...</source>
        <translation>Skriver ut…</translation>
    </message>
    <message>
        <location line="+89"/>
        <source>Print preview</source>
        <translation>Förhandsgranskning</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Booklet print preview</source>
        <translation>Förhandsgranska häfte</translation>
    </message>
</context>
<context>
    <name>EntriesViewHelper</name>
    <message>
        <location filename="../src/gui/EntriesViewHelper.cc" line="+42"/>
        <source>&amp;Print...</source>
        <translation>&amp;Skriv ut…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Print p&amp;review...</source>
        <translation>Förhandsgranska…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Print &amp;booklet...</source>
        <translation>Skriv ut &amp;häfte…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Export as &amp;HTML...</source>
        <translation>Exportera som &amp;HTML…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Booklet pre&amp;view...</source>
        <translation>Förhands&amp;granska häfte…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Export as &amp;TSV...</source>
        <translation>Exportera som &amp;TSV…</translation>
    </message>
    <message>
        <location line="+57"/>
        <source>Abort</source>
        <translation>Avbryt</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Selecting entries...</source>
        <translation>Väljer uppslag…</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Please wait...</source>
        <translation>Var god vänta…</translation>
    </message>
    <message>
        <location line="+259"/>
        <source>Print preview</source>
        <translation>Förhandsgranskning</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Booklet print</source>
        <translation>Skriv ut häfte</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Booklet print preview</source>
        <translation>Förhandsgranska häfte</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Export to tab-separated file...</source>
        <translation>Exportera som tabbavgränsad fil…</translation>
    </message>
    <message>
        <location line="+4"/>
        <location line="+47"/>
        <source>Cannot write file</source>
        <translation>Kan inte skriva till fil</translation>
    </message>
    <message>
        <location line="-47"/>
        <source>Unable to write file %1.</source>
        <translation>Kan inte skriva till filen %1.</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Error writing file</source>
        <translation>Kunde inte skriva till fil</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Error while writing file %1.</source>
        <translation>Fel vid skrivning av filen %1.</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Export to HTML flashcard file...</source>
        <translation>Exportera övningskort som HTML-fil...</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Unable to write file %1!</source>
        <translation>Kan inte skriva till filen %1!</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Cannot open template file</source>
        <translation>Kan inte öppna mall</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unable to open template file!</source>
        <translation>Kan inte öppna mall!</translation>
    </message>
    <message>
        <location line="-94"/>
        <location filename="../src/gui/EntriesViewHelper.h" line="+90"/>
        <source>Print</source>
        <translation>Skriv ut</translation>
    </message>
</context>
<context>
    <name>EntryDelegatePreferences</name>
    <message>
        <location filename="../src/gui/EntryDelegatePreferences.ui" line="+19"/>
        <source>Style:</source>
        <translation>Stil:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>One &amp;line</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+7"/>
        <source>&amp;Two lines</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="-196"/>
        <source>Main writing</source>
        <translation>Huvudstavning</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Readings and alternate writings</source>
        <translation>Läsningar och alternativa stavningar</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Definitions</source>
        <translation>Definitioner</translation>
    </message>
</context>
<context>
    <name>EntryDragButton</name>
    <message>
        <location filename="../src/gui/ToolBarDetailedView.cc" line="+29"/>
        <source>Drag the currently displayed entry</source>
        <translation>Dra det aktuella uppslaget</translation>
    </message>
</context>
<context>
    <name>EntryFormatter</name>
    <message>
        <location filename="../src/gui/EntryFormatter.cc" line="+57"/>
        <source>Cannot find detailed view HTML file!</source>
        <translation>Kan inte hitta HTML-filen för detaljerad visning.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Cannot find detailed view CSS file!</source>
        <translation>Kan inte hitta CSS-filen för detaljerad visning.</translation>
    </message>
    <message>
        <location line="+181"/>
        <source>Root list</source>
        <translation>Rotlista</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>Studied since %1.</source>
        <translation>Studerad sedan %1.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Score: %1.</source>
        <translation>Poäng: %1.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Last trained on %1.</source>
        <translation>Senast övad %1.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Last mistaken on %1.</source>
        <translation>Svarade fel senast %1.</translation>
    </message>
</context>
<context>
    <name>EntryListView</name>
    <message>
        <location filename="../src/gui/EntryListView.cc" line="+36"/>
        <source>Delete</source>
        <translation>Ta bort</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>New list...</source>
        <translation>Ny lista…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Go up</source>
        <translation>Gå upp</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Rename list...</source>
        <translation>Byt namn på lista…</translation>
    </message>
    <message>
        <location line="+93"/>
        <source>Unable to create list</source>
        <translation>Kan inte skapa lista</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>A database error occured while trying to add the list.</source>
        <translation>Ett databasfel inträffade under försök att lägga till listan.</translation>
    </message>
    <message>
        <location line="+39"/>
        <source>Confirm deletion</source>
        <translation>Bekräfta borttagning</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>This will delete the selected lists items and lists, including all their children. Continue?</source>
        <translation>Det här kommer att ta bort markerade listor, uppslag och allt som de innehåller. Är du säker?</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Removal failed</source>
        <translation>Kunde inte ta bort</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>A database error has occured while trying to remove the selected items:

%1

 Some of them may be remaining.</source>
        <translation>Ett databasfel uppstod när följande försökte tas bort:

%1

Det är möjligt att det inte togs bort.</translation>
    </message>
</context>
<context>
    <name>EntryListWidget</name>
    <message>
        <location filename="../src/gui/EntryListWidget.h" line="+33"/>
        <source>Lists</source>
        <translation>Listor</translation>
    </message>
</context>
<context>
    <name>EntryMenu</name>
    <message>
        <location filename="../src/gui/EntryMenu.cc" line="+28"/>
        <source>Add to &amp;study list</source>
        <translation>Lägg till &amp;studielistan</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Remove from &amp;study list</source>
        <translation>Ta bort från &amp;studielistan</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Already &amp;known</source>
        <translation>&amp;Kan redan</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>&amp;Reset score</source>
        <translation>&amp;Återställ poäng</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Set &amp;tags...</source>
        <translation>Ange &amp;taggar…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>&amp;Add tags...</source>
        <translation>&amp;Lägg till taggar…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Edit &amp;notes...</source>
        <translation>Redigera &amp;anteckningar…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Recently added tags...</source>
        <translation>Nyligen använda taggar…</translation>
    </message>
    <message>
        <location line="+87"/>
        <location line="+6"/>
        <source>Copy &quot;%1&quot; to clipboard</source>
        <translation>Kopiera ”%1” till urklipp</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Add &quot;%1&quot;</source>
        <translation>Lägg till ”%1”</translation>
    </message>
</context>
<context>
    <name>EntryNotesModel</name>
    <message>
        <location filename="../src/gui/EditEntryNotesDialog.cc" line="-65"/>
        <source>Notes</source>
        <translation>Anteckningar</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>&lt;New note&gt;</source>
        <translation>&lt;Ny anteckning&gt;</translation>
    </message>
</context>
<context>
    <name>EntryTypeFilterWidget</name>
    <message>
        <location filename="../src/gui/EntryTypeFilterWidget.cc" line="+33"/>
        <source>All</source>
        <translation>Alla</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Vocabulary</source>
        <translation>Ordlista</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Characters</source>
        <translation>Tecken</translation>
    </message>
    <message>
        <location filename="../src/gui/EntryTypeFilterWidget.h" line="+43"/>
        <source>Entry type filter</source>
        <translation>Filter för uppslagstyp</translation>
    </message>
</context>
<context>
    <name>FindHomographsJob</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictEntryFormatter.cc" line="+625"/>
        <source>Homographs</source>
        <translation>Homografer</translation>
    </message>
</context>
<context>
    <name>FindHomophonesJob</name>
    <message>
        <location line="-23"/>
        <source>Homophones</source>
        <translation>Homofoner</translation>
    </message>
</context>
<context>
    <name>FindVerbBuddyJob</name>
    <message>
        <location line="-15"/>
        <source>Transitive buddy</source>
        <translation>Transitiv verbpartikel</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Intransitive buddy</source>
        <translation>Intransitiv verbpartikel</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Buddy</source>
        <translation>Verbpartikel</translation>
    </message>
</context>
<context>
    <name>GeneralPreferences</name>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="-166"/>
        <source>English</source>
        <translation>Engelska</translation>
    </message>
    <message>
        <location line="-1"/>
        <source>German</source>
        <translation>Tyska</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>French</source>
        <translation>Franska</translation>
    </message>
    <message>
        <location line="-3"/>
        <source>Spanish</source>
        <translation>Spanska</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Norvegian Bokmal</source>
        <translation>Norska (bokmål)</translation>
    </message>
    <message>
        <location line="-11"/>
        <source>Czech</source>
        <translation>Tjeckiska</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Dutch</source>
        <translation>Nederländska</translation>
    </message>
    <message>
        <location line="-3"/>
        <source>Italian</source>
        <translation>Italienska</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Portuguese (Brazil)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Russian</source>
        <translation>Ryska</translation>
    </message>
    <message>
        <location line="-5"/>
        <source>Japanese</source>
        <translation>Japanska</translation>
    </message>
    <message>
        <location line="-11"/>
        <source>Arabic</source>
        <translation>Arabiska</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Persian (Iran)</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Finnish (Finland)</source>
        <translation>Finska (Finland)</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Hungarian</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Indonesian</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+5"/>
        <source>Polish</source>
        <translation>Polska</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Swedish</source>
        <translation>Svenska</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Thai</source>
        <translation>Thailändska</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Turkish</source>
        <translation>Turkiska</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ukrainian</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+1"/>
        <source>Vietnamese</source>
        <translation>Vietnamesiska</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Chinese</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+24"/>
        <source>General</source>
        <translation>Allmänt</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Application-wide default font</source>
        <translation>Förvalt typsnitt</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Monday</source>
        <translation>Måndag</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sunday</source>
        <translation>Söndag</translation>
    </message>
    <message>
        <location line="+48"/>
        <source>Next check: %1</source>
        <translation>Nästa koll: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/GeneralPreferences.ui" line="+17"/>
        <source>General settings</source>
        <translation>Allmänna inställningar</translation>
    </message>
    <message>
        <location line="+35"/>
        <source>System default</source>
        <translation>Systemstandard</translation>
    </message>
    <message>
        <location line="-27"/>
        <source>Preferred dictionary language</source>
        <translation>Föredraget språk i ordboken</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Preferred &amp;GUI language</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+34"/>
        <source>The dictionaries will use this language whenever possible. English will be used as a fallback.</source>
        <translation>Det här språket kommer att användas i ordböckerna när möjligt, i andra fall används engelska.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Same as GUI</source>
        <translation>Samma som användargränssnittet</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>You need to restart Tagaini Jisho for these changes to take effect.</source>
        <translation>Tagaini jisho måste startas om för att dessa ändringar ska börja gälla.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Dates</source>
        <translation>Datum</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Updates</source>
        <translation>Uppdateringar</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Check for updates</source>
        <translation>Leta efter uppdateringar</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>every day</source>
        <translation>varje dag</translation>
    </message>
    <message>
        <location line="+3"/>
        <source> days</source>
        <translation> dagar</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>every </source>
        <translation>var</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Also check for development versions</source>
        <translation>Leta även efter utvecklingsversioner</translation>
    </message>
    <message>
        <location line="-113"/>
        <source>The GUI will use this language whenever possible. English will be used as a fallback.</source>
        <translation>Användargränssnittet kommer att använda detta språk, där detta inte är möjligt används engelska.</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>If checked, English meanings will be shown even if a translation in the preferred language is available.</source>
        <translation>Om vald, kommer engelska översättningar att visas även om översättningar på det föredragna språket finns tillgängliga.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Always show English meanings</source>
        <translation>Visa alltid engelska översättningar</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Wee&amp;ks start on</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+59"/>
        <source>Next check:</source>
        <translation>Nästa koll:</translation>
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
        <translation>Alternativa läsningar: </translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Alternate writings:</source>
        <translation>Alternativa stavningar:</translation>
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
        <translation>Exempelmeningar</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>JMdict ID</source>
        <translation>JMdict-id</translation>
    </message>
</context>
<context>
    <name>JMdictFilterWidget</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictFilterWidget.cc" line="+42"/>
        <source>Using studied kanji only</source>
        <translation>Använd endast studerade kanji</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>With kanji:</source>
        <translation>Med kanji:</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>With components:</source>
        <translation>Med komponenter:</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Include kana-only words</source>
        <translation>Inkludera ord med endast kana</translation>
    </message>
    <message>
        <location line="+13"/>
        <location line="+117"/>
        <source>Part of speech</source>
        <translation>Grammatik</translation>
    </message>
    <message>
        <location line="-111"/>
        <source>Dialect</source>
        <translation>Dialekt</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Field</source>
        <translation>Terminologi</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Misc</source>
        <translation>Övrigt</translation>
    </message>
    <message>
        <location line="+53"/>
        <source> with </source>
        <translation> med </translation>
    </message>
    <message>
        <location line="+9"/>
        <source> with component </source>
        <translation> med komponenten </translation>
    </message>
    <message>
        <location line="+9"/>
        <source> with studied kanji</source>
        <translation> med studerade kanji</translation>
    </message>
    <message>
        <location line="+4"/>
        <source> using kana only</source>
        <translation> med endast kana</translation>
    </message>
    <message>
        <location line="-5"/>
        <source>, studied kanji only</source>
        <translation>, endast studerade kanji</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>, including kana words</source>
        <translation>, inklusive kana-ord</translation>
    </message>
    <message>
        <location line="+9"/>
        <location line="+1"/>
        <source>Vocabulary</source>
        <translation>Ordlista</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Pos:</source>
        <translation>Gram:</translation>
    </message>
</context>
<context>
    <name>JMdictGUIPlugin</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictGUIPlugin.cc" line="+61"/>
        <source>Vocabulary flashcards</source>
        <translation>Övningskort för ordlista</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;japanese, whole study list</source>
        <translation>Från &amp;japanska, hela studielistan</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;japanese, current set</source>
        <translation>Från &amp;japanska, aktuellt set</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>From &amp;translation, whole study list</source>
        <translation>Från &amp;översättning, hela studielistan</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;translation, current set</source>
        <translation>Från &amp;översättning, aktuellt set</translation>
    </message>
    <message>
        <location line="+104"/>
        <location line="+19"/>
        <source>Nothing to train</source>
        <translation>Inget att öva</translation>
    </message>
    <message>
        <location line="-19"/>
        <location line="+19"/>
        <source>There are no vocabulary entries in this set to train on.</source>
        <translation>Det finns inga uppslag i ordlistan att öva i det här setet.</translation>
    </message>
</context>
<context>
    <name>JMdictLongDescs</name>
    <message>
        <location filename="../3rdparty/JMdictDescs.cc" line="+67"/>
        <source>military</source>
        <translation>militär</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>mathematics</source>
        <translation>matematik</translation>
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
        <translation>Kyushu-ben</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Ryuukyuu-ben</source>
        <translation>Ryukyu-ben</translation>
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
        <translation>Tohoku-ben</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>Kantou-ben</source>
        <translation>Kanto-ben</translation>
    </message>
    <message>
        <location line="+126"/>
        <source>manga slang</source>
        <translation>mangaslang</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>polite (teineigo) language</source>
        <translation>artigt språk (teineigo)</translation>
    </message>
    <message>
        <location line="+105"/>
        <source>su verb - precursor to the modern suru</source>
        <translation>su-verb – föregångare till det moderna suru</translation>
    </message>
    <message>
        <location line="-245"/>
        <source>Hokkaido-ben</source>
        <translation>Hokkaido-ben</translation>
    </message>
    <message>
        <location line="+154"/>
        <source>vulgar expression or word</source>
        <translation>Vulgära uttryck</translation>
    </message>
    <message>
        <location line="-29"/>
        <source>idiomatic expression</source>
        <translation>idiomatiskt uttryck</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>honorific or respectful (sonkeigo) language</source>
        <translation>respektfullt språk (sonkeigo)</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>female term or language</source>
        <translation>kvinnligt språk</translation>
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
        <translation>fonetisk läsning (ateji)</translation>
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
        <translation>barnspråk</translation>
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
        <translation>kopula</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>expressions (phrases, clauses, etc.)</source>
        <translation>uttryck (fraser, meningar m. m.)</translation>
    </message>
    <message>
        <location line="+73"/>
        <source>gikun (meaning as reading) or jukujikun (special kanji reading)</source>
        <translation>gikun (betydelse som uttal) eller jukujikun (specialuttal)</translation>
    </message>
    <message>
        <location line="-152"/>
        <source>word containing irregular kanji usage</source>
        <translation>oregelbunden användning av kanji</translation>
    </message>
    <message>
        <location line="-1"/>
        <location line="+154"/>
        <source>word containing irregular kana usage</source>
        <translation>oregelbunden användning av kana</translation>
    </message>
    <message>
        <location line="-152"/>
        <source>irregular okurigana usage</source>
        <translation>oregelbunden användning av okurigana</translation>
    </message>
    <message>
        <location line="+153"/>
        <source>out-dated or obsolete kana usage</source>
        <translation>föråldrad användning av kana</translation>
    </message>
    <message>
        <location line="-118"/>
        <source>onomatopoeic or mimetic word</source>
        <translation>onomatopoetiskt eller mimetiskt ord</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>poetical term</source>
        <translation>poesi</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>proverb</source>
        <translation>ordspråk</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>quotation</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+3"/>
        <source>sensitive</source>
        <translation>känsligt uttryck</translation>
    </message>
    <message>
        <location line="+44"/>
        <source>unclassified</source>
        <translation>oklassifierad</translation>
    </message>
    <message>
        <location line="-33"/>
        <source>yojijukugo</source>
        <translation>yojikukugo</translation>
    </message>
    <message>
        <location line="+36"/>
        <source>Ichidan verb - kureru special class</source>
        <translation>ichidan-verb – speciell kureru-klass</translation>
    </message>
    <message>
        <location line="+53"/>
        <source>suru verb - included</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="-82"/>
        <source>archaic/formal form of na-adjective</source>
        <translation>ålderdomlig/formell form av na-adjektiv</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>proper noun</source>
        <translation>egennamn</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>verb unspecified</source>
        <translation>ospecificerat verb</translation>
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
        <translation>adjektiv (keiyoushi) – yoi/ii-klassen</translation>
    </message>
    <message>
        <location line="-34"/>
        <source>jocular, humorous term</source>
        <translation>humor</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>humble (kenjougo) language</source>
        <translation>ödmjukt språk (kenjougo)</translation>
    </message>
    <message>
        <location line="-21"/>
        <source>abbreviation</source>
        <translation>förkortning</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>familiar language</source>
        <translation>familjärt språk</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>rude or X-rated term (not displayed in educational software)</source>
        <translation>grovt eller fräckt uttryck (visas vanligen inte i pedagogiska sammanhang)</translation>
    </message>
    <message>
        <location line="-44"/>
        <source>derogatory</source>
        <translation>nedlåtande språk</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>word usually written using kana alone</source>
        <translation>ord som vanligen skrivs med endast kana</translation>
    </message>
    <message>
        <location line="-3"/>
        <source>slang</source>
        <translation>slang</translation>
    </message>
    <message>
        <location line="-20"/>
        <source>male term or language</source>
        <translation>manligt uttryck</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>obsolete term</source>
        <translation>föråldrat uttryck</translation>
    </message>
    <message>
        <location line="+51"/>
        <source>noun (temporal) (jisoumeishi)</source>
        <translation>temporalt substantiv (jisoumeishi)</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>prefix</source>
        <translation>prefix</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>interjection (kandoushi)</source>
        <translation>interjektion (kandoushi)</translation>
    </message>
    <message>
        <location line="+61"/>
        <source>Godan verb - Uru old class verb (old form of Eru)</source>
        <translation>godan-verb – gammal uru-klass (äldre form av eru)</translation>
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
        <translation>intransitivt verb</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Kuru verb - special class</source>
        <translation>kuru-verb – speciell klass</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>irregular nu verb</source>
        <translation>oregelbundet nu-verb</translation>
    </message>
    <message>
        <location line="-54"/>
        <source>particle</source>
        <translation>partikel</translation>
    </message>
    <message>
        <location line="+55"/>
        <source>irregular ru verb, plain form ends with -ri</source>
        <translation>oregelbundet ru-verb (grundform har ri-ändelse)</translation>
    </message>
    <message>
        <location line="-85"/>
        <source>noun or verb acting prenominally</source>
        <translation>prenominalt substantiv eller verb</translation>
    </message>
    <message>
        <location line="+90"/>
        <source>transitive verb</source>
        <translation>transitivt verb</translation>
    </message>
    <message>
        <location line="-65"/>
        <source>noun, used as a suffix</source>
        <translation>substantiv, som suffix</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>conjunction</source>
        <translation>konjunktion</translation>
    </message>
    <message>
        <location line="-15"/>
        <source>adjective (keiyoushi)</source>
        <translation>adjektiv (keiyoushi)</translation>
    </message>
    <message>
        <location line="+90"/>
        <source>Ichidan verb - zuru verb (alternative form of -jiru verbs)</source>
        <translation>ichidan-verb – zuru-verb (alternativ form av jiru-verb)</translation>
    </message>
    <message>
        <location line="-80"/>
        <source>adverb (fukushi)</source>
        <translation>adverb (fukushi)</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>suffix</source>
        <translation>suffix</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>Godan verb - -aru special class</source>
        <translation>godan-verb – speciell aru-klass</translation>
    </message>
    <message>
        <location line="-55"/>
        <source>auxiliary</source>
        <translation>hjälpverb</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>auxiliary verb</source>
        <translation>hjälpverb</translation>
    </message>
    <message>
        <location line="-10"/>
        <source>adjectival nouns or quasi-adjectives (keiyodoshi)</source>
        <translation>nominaladjektiv (keiyodoshi)</translation>
    </message>
    <message>
        <location line="+67"/>
        <source>Godan verb - Iku/Yuku special class</source>
        <translation>godan-verb – speciell iku/yuku-klass</translation>
    </message>
    <message>
        <location line="-38"/>
        <source>Ichidan verb</source>
        <translation>Ichidan-verb</translation>
    </message>
    <message>
        <location line="-10"/>
        <source>noun, used as a prefix</source>
        <translation>substantiv, som prefix</translation>
    </message>
    <message>
        <location line="-10"/>
        <source>auxiliary adjective</source>
        <translation>hjälpadjektiv</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>numeric</source>
        <translation>numerisk</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>counter</source>
        <translation>räknare</translation>
    </message>
    <message>
        <location line="-10"/>
        <source>pre-noun adjectival (rentaishi)</source>
        <translation>attributivt adjektiv (rentaishi)</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Nidan verb with &apos;u&apos; ending (archaic)</source>
        <translation>nidan-verb med u-ändelse (ålderdomlig)</translation>
    </message>
    <message>
        <location line="+53"/>
        <source>suru verb - special class</source>
        <translation>suru-verb (speciell klass)</translation>
    </message>
    <message>
        <location line="-68"/>
        <source>noun (common) (futsuumeishi)</source>
        <translation>substantiv (futsuumeishi)</translation>
    </message>
    <message>
        <location line="+65"/>
        <source>noun or participle which takes the aux. verb suru</source>
        <translation>substantiv eller partikel som tar hjälpverbet suru</translation>
    </message>
    <message>
        <location line="-64"/>
        <source>adverbial noun (fukushitekimeishi)</source>
        <translation>Adverbialsubstantiv (fukushitekimeishi)</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>pronoun</source>
        <translation>pronomen</translation>
    </message>
</context>
<context>
    <name>JMdictPreferences</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictPreferences.cc" line="+24"/>
        <source>Vocabulary entries</source>
        <translation>Uppslag i ordlistan</translation>
    </message>
    <message>
        <location filename="../src/gui/jmdict/JMdictPreferences.ui" line="+14"/>
        <source>Form</source>
        <translation>Formulär</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Display</source>
        <translation>Visning</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Basic information</source>
        <translation>Grundläggande information</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>JLPT level</source>
        <translation>JLPT-nivå</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Kanji used in main reading</source>
        <translation>Kanji som används i huvudläsning</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>JMdict ID (only for debugging)</source>
        <translation>JMdict-id (felsökning)</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Additional information</source>
        <translation>Vidare information</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>For transitive verbs, look for the equivalent intransitive entry and vice-versa.</source>
        <translation>Se uppslagen för både det transitiva och det intransitiva verbet.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Search for transitive/intransitive verb buddy</source>
        <translation>Sök (in)transitiva verbpartiklar</translation>
    </message>
    <message>
        <location line="+12"/>
        <location line="+24"/>
        <source>Display words that have the same pronunciation.</source>
        <translation>Visa ord som har samma uttal.</translation>
    </message>
    <message>
        <location line="-21"/>
        <source>Homophones</source>
        <translation>Homofoner</translation>
    </message>
    <message>
        <location line="+7"/>
        <location line="+24"/>
        <source>Studied only</source>
        <translation>Endast studerade</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Homographs</source>
        <translation>Homografer</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Link to example sentences:</source>
        <translation>Länk till exempelmeningar:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>If enabled, display a link that will look for example sentences in your web browser using the selected service</source>
        <translation>Om aktiverad, visa en länk för att söka efter exempelmeningar i din vävläsare med vald tjänst</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Disabled</source>
        <translation>Avaktiverad</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Printing</source>
        <translation>Utskrift</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>General</source>
        <translation>Allmänt</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Header font size:</source>
        <translation>Typgrad för rubrik:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Maximum number of definitions:</source>
        <translation>Maximalt antal definitioner</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>All</source>
        <translation>Alla</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Kanji</source>
        <translation>Kanji</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kanji used in word</source>
        <translation>Kanji som ingår i ord</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Only studied kanji</source>
        <translation>Endast studerade kanji</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Preview</source>
        <translation>Förhandsgranskning</translation>
    </message>
    <message>
        <location line="+142"/>
        <source>Definitions filtering</source>
        <translation>Filtrering av definitioner</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Filtered definitions</source>
        <translation>Filtrerade definitioner</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Displayed:</source>
        <translation>Visas:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Definitions types in this list will be searched for and displayed.</source>
        <translation>Följande definitionstyper kommer att visas.</translation>
    </message>
    <message>
        <location line="+27"/>
        <location line="+24"/>
        <source>...</source>
        <translation>…</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Filtered:</source>
        <translation>Visas inte:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Definitions types in this list will be ignored when searching and displaying entries.</source>
        <translation>Följande definitionstyper kommer inte att visas.</translation>
    </message>
</context>
<context>
    <name>JMdictYesNoTrainer</name>
    <message>
        <location filename="../src/gui/jmdict/JMdictYesNoTrainer.cc" line="+23"/>
        <source>Show &amp;furigana</source>
        <translation>Visa &amp;furigana</translation>
    </message>
</context>
<context>
    <name>KanaSelector</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanaSelector.ui" line="+14"/>
        <source>Form</source>
        <translation>Formulär</translation>
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
        <translation>Föråldrad</translation>
    </message>
</context>
<context>
    <name>KanaView</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanaView.cc" line="+162"/>
        <source>Select All</source>
        <translation>Välja alla</translation>
    </message>
</context>
<context>
    <name>KanjiComponentWidget</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPopup.cc" line="+100"/>
        <source>%1 (drawn as &quot;%2&quot;)</source>
        <translation>%1 (som ”%2”)</translation>
    </message>
</context>
<context>
    <name>KanjiInputPopupAction</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiSelector.cc" line="+398"/>
        <source>Triggers the kanji input panel</source>
        <translation>Aktiverar kanji-inmatning</translation>
    </message>
</context>
<context>
    <name>KanjiPlayer</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPlayer.cc" line="+50"/>
        <source>Play</source>
        <translation>Spela</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pause</source>
        <translation>Pausa</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Stop</source>
        <translation>Stoppa</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Next stroke</source>
        <translation>Nästa penseldrag</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Previous stroke</source>
        <translation>Föregående penseldrag</translation>
    </message>
</context>
<context>
    <name>KanjiPopup</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPopup.ui" line="+81"/>
        <source>See in detailed view</source>
        <translation>Detaljerad visning…</translation>
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
        <translation>&lt;b&gt;Frekv:&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>&lt;b&gt;Grade:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;Klass:&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>&lt;b&gt;JLPT:&lt;/b&gt; N%1&lt;br/&gt;</source>
        <translation>&lt;b&gt;JLPT:&lt;/b&gt; N%1&lt;br/&gt;</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>&lt;b&gt;Components:&lt;/b&gt; %2 %1</source>
        <translation>&lt;b&gt;Komponenter:&lt;/b&gt; %2 %1</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>&lt;b&gt;Radicals:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;Radikaler:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="-28"/>
        <source>&lt;b&gt;Score:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;Poäng:&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPopup.ui" line="-61"/>
        <source>Form</source>
        <translation>Formulär</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Previous entry</source>
        <translation>Föregående uppslag</translation>
    </message>
    <message>
        <location line="+3"/>
        <location line="+20"/>
        <location line="+30"/>
        <location line="+20"/>
        <location line="+23"/>
        <source>...</source>
        <translation>…</translation>
    </message>
    <message>
        <location line="-76"/>
        <source>Next entry</source>
        <translation>Nästa uppslag</translation>
    </message>
    <message>
        <location line="+50"/>
        <source>Entry options menu</source>
        <translation>Valmeny för uppslag</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Close this popup</source>
        <translation>Stäng den här rutan</translation>
    </message>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiPopup.cc" line="-36"/>
        <source>No information about this kanji!</source>
        <translation>Ingen information om det här tecknet!</translation>
    </message>
</context>
<context>
    <name>KanjiSelector</name>
    <message>
        <location filename="../src/gui/kanjidic2/KanjiSelector.ui" line="+6"/>
        <source>Radical search</source>
        <translation>Sök med radikaler</translation>
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
        <translation>&lt;b&gt;Streck:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>&lt;b&gt;Frequency:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;Frekvens:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>&lt;b&gt;Grade:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;Nivå:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>&lt;b&gt;4 corner:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;4 hörn:&lt;/b&gt; %1</translation>
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
        <translation>Streck</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Frequency</source>
        <translation>Frekvens</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Grade</source>
        <translation>Nivå</translation>
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
        <translation>Varianter</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Variation of</source>
        <translation>Variant av</translation>
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
        <translation>4 hörn</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Radicals</source>
        <translation>Radikaler</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Components</source>
        <translation>Komponenter</translation>
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
        <translation>Antal streck</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Range</source>
        <translation>Omfång</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Radicals</source>
        <translation>Radikaler</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Components</source>
        <translation>Komponenter</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Unicode</source>
        <translation>Unicode</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>SKIP code</source>
        <translation>SKIP-kod</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Four corner</source>
        <translation>Fyra hörn</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>School grade</source>
        <translation>Skolnivå</translation>
    </message>
    <message>
        <location line="+3"/>
        <location line="+261"/>
        <source>Grade</source>
        <translation>Klass</translation>
    </message>
    <message>
        <location line="-247"/>
        <source>All &amp;Kyouiku kanji</source>
        <translation>Samtliga &amp;kyouiku-kanji</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>All &amp;Jouyou kanji</source>
        <translation>Samtliga &amp;jouyou-kanji</translation>
    </message>
    <message>
        <location line="+219"/>
        <source>, %1 strokes</source>
        <translation>, %1 streck</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>, strokes&lt;=%1</source>
        <translation>, streck ≤ %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>, strokes&gt;=%1</source>
        <translation>, streck ≥ %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>, %1-%2 strokes</source>
        <translation>, %1–%2 streck</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>, unicode: %1</source>
        <translation>, unicode: %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>, skip: %1</source>
        <translation>, SKIP: %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>, 4c: %1</source>
        <translation>, 4h: %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>, grade: %1</source>
        <translation>, nivå: %1</translation>
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
        <translation>Klass:</translation>
    </message>
</context>
<context>
    <name>Kanjidic2GUIPlugin</name>
    <message>
        <location filename="../src/gui/kanjidic2/Kanjidic2GUIPlugin.cc" line="+49"/>
        <location line="+7"/>
        <source>Invalid</source>
        <translation>Ogiltig</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>Elementary 1st grade</source>
        <translation>1:a klass</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 2nd grade</source>
        <translation>2:a klass</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 3rd grade</source>
        <translation>3:e klass</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 4th grade</source>
        <translation>4:e klass</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 5th grade</source>
        <translation>5:e klass</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Elementary 6th grade</source>
        <translation>6:e klass</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Junior high school</source>
        <translation>Högstadium</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Used for names (1)</source>
        <translation>Används i namn (1)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Used for names (2)</source>
        <translation>Används i namn (2)</translation>
    </message>
    <message>
        <location line="+33"/>
        <source>Character flashcards</source>
        <translation>Övningskort för kanji</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;character, whole study list</source>
        <translation>Från &amp;tecken, hela studielistan</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;character, current set</source>
        <translation>Från &amp;tecken, aktuellt set</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>From &amp;meaning, whole study list</source>
        <translation>Från &amp;betydelse, hela studielistan</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>From &amp;meaning, current set</source>
        <translation>Från &amp;betydelse, aktuellt set</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>&amp;Reading practice, whole study list</source>
        <translation>&amp;Läsningsträning, hela studielistan</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kana</source>
        <translation>Kana</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Radical search input</source>
        <translation>Radikaler att söka med</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Component search input</source>
        <translation>Komponenter som ingår</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Show stroke popup for currently displayed kanji</source>
        <translation>Visa ruta med streckordning för nuvarande kanji</translation>
    </message>
    <message>
        <location line="+94"/>
        <location line="+18"/>
        <source>Nothing to train</source>
        <translation>Inget att öva</translation>
    </message>
    <message>
        <location line="-18"/>
        <location line="+18"/>
        <source>There are no kanji entries in this set to train on.</source>
        <translation>Det finns inga kanji att öva i det här setet.</translation>
    </message>
    <message>
        <location line="+186"/>
        <source>Open in detailed view...</source>
        <translation>Öppna detaljerad visning…</translation>
    </message>
    <message>
        <location line="+25"/>
        <source>Tracing for %1</source>
        <translation>Konturer för %1</translation>
    </message>
</context>
<context>
    <name>Kanjidic2Preferences</name>
    <message>
        <location filename="../src/gui/kanjidic2/Kanjidic2Preferences.ui" line="+14"/>
        <source>Form</source>
        <translation>Formulär</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Display</source>
        <translation>Visning</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Basic information</source>
        <translation>Grundläggande information</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Readings</source>
        <translation>Läsningar</translation>
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
        <translation>Variationer</translation>
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
        <translation>Frekvens</translation>
    </message>
    <message>
        <location line="-469"/>
        <location line="+476"/>
        <source>School grade</source>
        <translation>Skolnivå</translation>
    </message>
    <message>
        <location line="-483"/>
        <location line="+490"/>
        <source>JLPT level</source>
        <translation>JLPT-nivå</translation>
    </message>
    <message>
        <location line="-458"/>
        <location line="+116"/>
        <source>Components</source>
        <translation>Komponenter</translation>
    </message>
    <message>
        <location line="-197"/>
        <location line="+511"/>
        <source># of strokes</source>
        <translation>Antal streck</translation>
    </message>
    <message>
        <location line="-518"/>
        <location line="+504"/>
        <source>SKIP code</source>
        <translation>SKIP-kod</translation>
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
        <translation>Fyrahörnsmetoden</translation>
    </message>
    <message>
        <location line="-504"/>
        <location line="+511"/>
        <source>Heisig number</source>
        <translation>Heisig-nummer</translation>
    </message>
    <message>
        <location line="-487"/>
        <source>Additional information</source>
        <translation>Ytterligare information</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Radicals</source>
        <translation>Radikaler</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Dictionaries</source>
        <translation type="unfinished"/>
    </message>
    <message>
        <location line="+11"/>
        <source>Compounds:</source>
        <translation>Sammansättningar:</translation>
    </message>
    <message>
        <location line="+14"/>
        <location line="+25"/>
        <source>Studied only</source>
        <translation>Endast studerade</translation>
    </message>
    <message>
        <location line="-14"/>
        <source>Words using the kanji:</source>
        <translation>Ord som använder tecknet:</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>Printing</source>
        <translation>Utskrift</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Kanji information</source>
        <translation>Kanji-information</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Meaning</source>
        <translation>Betydelse</translation>
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
        <translation>Endast studerade komponenter</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>words</source>
        <translation>ord</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Only studied words</source>
        <translation>Endast studerade ord</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Kanji drawing style</source>
        <translation>Stil för kanji</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Handwriting</source>
        <translation>Handstil</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Kanji header size:</source>
        <translation>Rubrikstorlek för kanji:</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Print grid</source>
        <translation>Skriv ut gitter</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Show stroke numbers</source>
        <translation>Visa antal streck</translation>
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
        <translation>Förhandsgranskning</translation>
    </message>
    <message>
        <location line="-322"/>
        <source>Tooltip</source>
        <translation>Inforuta</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Score</source>
        <translation>Poäng</translation>
    </message>
    <message>
        <location line="+73"/>
        <source>Stroke animation</source>
        <translation>Streckdragning</translation>
    </message>
    <message>
        <location line="+181"/>
        <source> seconds</source>
        <translation> sekunder</translation>
    </message>
    <message>
        <location line="+32"/>
        <source>Kana selector</source>
        <translation>Kana-väljare</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Appearance</source>
        <translation>Utseende</translation>
    </message>
    <message>
        <location line="-484"/>
        <location line="+280"/>
        <source>Size:</source>
        <translation>Storlek:</translation>
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
        <translation>Skrivhastighet:</translation>
    </message>
    <message>
        <location line="+9"/>
        <location line="+31"/>
        <source>Default</source>
        <translation>Standard</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Pause between strokes:</source>
        <translation>Uppehåll mellan streck</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Show numbers</source>
        <translation>Visa strecknummer</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Show grid</source>
        <translation>Visa gitter</translation>
    </message>
    <message>
        <location line="+39"/>
        <source>Animation playback</source>
        <translation>Uppspelning</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Play animation when the kanji popup appears</source>
        <translation>Aktivera animering av streckordning när inforutan visas</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Loop animation after:</source>
        <translation>Spela upp igen efter:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Do not loop</source>
        <translation>Visa animering endast en gång</translation>
    </message>
    <message>
        <location filename="../src/gui/kanjidic2/Kanjidic2Preferences.cc" line="+27"/>
        <source>Character entries</source>
        <translation>Kanjiuppslag</translation>
    </message>
    <message>
        <location line="+43"/>
        <source>Character font</source>
        <translation>Typsnitt för kanji</translation>
    </message>
</context>
<context>
    <name>ListsViewPreferences</name>
    <message>
        <location filename="../src/gui/ListsViewPreferences.ui" line="+17"/>
        <source>Lists display</source>
        <translation>Visning av listor</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Smooth scrolling</source>
        <translation>Mjuk rullning</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Preview</source>
        <translation>Förhandsgranskning</translation>
    </message>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="+162"/>
        <source>Lists</source>
        <translation>Listor</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="+158"/>
        <source>&amp;Quit</source>
        <translation>&amp;Avsluta</translation>
    </message>
    <message>
        <location line="-114"/>
        <source>&amp;Program</source>
        <translation>&amp;Arkiv</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>&amp;Search</source>
        <translation>&amp;Sök</translation>
    </message>
    <message>
        <location line="-11"/>
        <source>&amp;Help</source>
        <translation>&amp;Hjälp</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>P&amp;ractice</source>
        <translation>&amp;Öva</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>S&amp;aved searches</source>
        <translation>S&amp;parade sökningar</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Lists</source>
        <translation>Listor</translation>
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
        <translation>&amp;Om</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Focus results list</source>
        <translation>Ge fokus till resultatlistan</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Ctrl+O</source>
        <translation>Ctrl-O</translation>
    </message>
    <message>
        <location line="-115"/>
        <source>Preferences...</source>
        <translation>Inställningar…</translation>
    </message>
    <message>
        <location line="+56"/>
        <source>&amp;Report a bug...</source>
        <translation>&amp;Rapportera bugg…</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>&amp;Suggest a feature...</source>
        <translation>&amp;Föreslå en ny funktion…</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Ask a &amp;question...</source>
        <translation>Ställ en &amp;fråga…</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Support Tagaini Jisho - Make a &amp;donation!</source>
        <translation>Stöd Tagaini jisho – &amp;donera!</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Focus text search</source>
        <translation>Ge fokus till sökfältet</translation>
    </message>
    <message>
        <location line="-122"/>
        <location filename="../src/gui/MainWindow.cc" line="+235"/>
        <location line="+7"/>
        <source>Export user data...</source>
        <translation>Exportera användardata…</translation>
    </message>
    <message>
        <location line="+9"/>
        <location filename="../src/gui/MainWindow.cc" line="+6"/>
        <source>Import user data...</source>
        <translation>Importera användardata…</translation>
    </message>
    <message>
        <location line="-123"/>
        <source>Tagaini Jisho</source>
        <translation>Tagaini jisho</translation>
    </message>
    <message>
        <location line="+80"/>
        <source>Search</source>
        <translation>Sök</translation>
    </message>
    <message>
        <location line="+75"/>
        <source>&amp;Save current search...</source>
        <translation>&amp;Spara aktuell sökning…</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>&amp;New folder...</source>
        <translation>&amp;Ny katalog…</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>&amp;Organize saved searches...</source>
        <translation>&amp;Hantera sparade sökningar…</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Whole study list train &amp;settings...</source>
        <translation>Inställningar för att öva med hela studielistan…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cc" line="-55"/>
        <source>Auto-search on clipboard content</source>
        <translation>Sök automatiskt på innehållet i urklipp</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Please consider supporting Tagaini Jisho</source>
        <translation>Ägna gärna en tanke på att stödja Tagaini jisho</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You have now been using Tagaini Jisho for a short while. If you like it, please show your support by making a donation that expresses your appreciation of this software. While Tagaini is free, its development has a cost and your support will have a positive influence on its future. You can donate via Paypal or credit card.

This message is a one-time reminder and will not appear anymore - you can still donate later by choosing the corresponding option in the Help menu.</source>
        <translation>Du har nu använt Tagaini jisho ett litet tag. Om du gillar programmet, överväg gärna att visa din uppskattning genom att göra en donation. Medan Tagaini är gratis kostar utvecklingen av detta program pengar och ditt stöd kommer att ha ett positivt inflytande över dess framtid. Du kan donera via Paypal eller med kreditkort.

Det här meddelandet kommer endast att visas denna gång – du kan välja att ge en gåva senare genom att välja motsvarande alternativ i Hjälp-menyn.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Donate!</source>
        <translation>Donera!</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Nah</source>
        <translation>Nä</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Error: Could not export database! Please verify that you have write permissions on the target file.</source>
        <translation>Fel: Kunde inte exportera databasen! Kontrollera att du kan skriva till målfilen.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>About to replace user data!</source>
        <translation>På väg att ersätta användardata!</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>&lt;b&gt;WARNING:&lt;/b&gt; All the user data (study list, tags, notes, training, ...) is going to be replaced by the selected user data file. Are you &lt;b&gt;sure&lt;/b&gt; you want to do this?</source>
        <translation>&lt;b&gt;VARNING:&lt;/b&gt; Alla användardata (studielista, taggar, anteckningar, övning, …) kommer att ersättas med den valda användardatafilen. Är du &lt;b&gt;säker&lt;/b&gt; att du vill fortsätta?</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Please restart Tagaini Jisho</source>
        <translation>Tagaini jisho behöver startas om</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The imported data will be available the next time you start Tagaini Jisho. All changes performed from now will be lost. Do you want to exit Tagaini Jisho now?</source>
        <translation>Importerade data kommer att vara tillgängliga nästa gång du startar Tagaini jisho. Allt du kommer att göra i fortsättningen under nuvarande programsession kommer att förloras. Vill du avsluta Tagaini jisho nu?</translation>
    </message>
    <message>
        <location line="+113"/>
        <source>A development update is available!</source>
        <translation>En ny utvecklingsversion finns tillgänglig!</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Development version %1 of Tagaini Jisho is available. Do you want to download it now?</source>
        <translation>Utvecklingsversion %1 av Tagaini jisho finns tillgänglig. Vill du hämta hem den nu?</translation>
    </message>
    <message>
        <location line="+54"/>
        <source>Save current search here...</source>
        <translation>Spara aktuell sökning här…</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Save current search</source>
        <translation>Spara aktuell sökning</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Please enter a name for this search:</source>
        <translation>Ange ett namn för den här sökningen:</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unnamed</source>
        <translation>Ej namngiven</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>New folder</source>
        <translation>Ny katalog</translation>
    </message>
    <message>
        <location line="-34"/>
        <source>Create new folder here...</source>
        <translation>Skapa ny katalog här…</translation>
    </message>
    <message>
        <location line="+34"/>
        <source>Please enter a name for this folder</source>
        <translation>Ange katalogens namn</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unnamed folder</source>
        <translation>Namnlös katalog</translation>
    </message>
    <message>
        <location line="-106"/>
        <source>An update is available!</source>
        <translation>En uppdatering finns tillgänglig!</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Version %1 of Tagaini Jisho is available. Do you want to download it now?</source>
        <translation>Version %1 av Tagaini jisho finns tillgänglig. Vill du hämta hem den nu?</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+15"/>
        <source>Let&apos;s go!</source>
        <translation>Kör på!</translation>
    </message>
    <message>
        <location line="-13"/>
        <location line="+15"/>
        <source>Maybe later</source>
        <translation>Kanske senare</translation>
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
        <translation>Återställ filter</translation>
    </message>
</context>
<context>
    <name>NotesFilterWidget</name>
    <message>
        <location filename="../src/gui/NotesFilterWidget.cc" line="+50"/>
        <source>Notes</source>
        <translation>Anteckningar</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Has note</source>
        <translation>Har anteckning</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Note contains %1</source>
        <translation>Anteckning innehåller %1</translation>
    </message>
</context>
<context>
    <name>PreferencesFontChooser</name>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="+156"/>
        <source>Change...</source>
        <translation>Ändra…</translation>
    </message>
    <message>
        <location line="-5"/>
        <source>%1:</source>
        <translation>%1:</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Default</source>
        <translation>Standard</translation>
    </message>
</context>
<context>
    <name>PreferencesWindow</name>
    <message>
        <location filename="../src/gui/PreferencesWindow.ui" line="+14"/>
        <source>Preferences</source>
        <translation>Inställningar</translation>
    </message>
</context>
<context>
    <name>RadicalSearchWidget</name>
    <message>
        <location filename="../src/gui/kanjidic2/RadicalSearchWidget.ui" line="+14"/>
        <source>Radical search</source>
        <translation>Sök med radikaler</translation>
    </message>
</context>
<context>
    <name>ReadingTrainer</name>
    <message>
        <location filename="../src/gui/ReadingTrainer.ui" line="+14"/>
        <location filename="../src/gui/ReadingTrainer.cc" line="+41"/>
        <source>Reading practice</source>
        <translation>Träna läsningar</translation>
    </message>
    <message>
        <location filename="../src/gui/ReadingTrainer.cc" line="+5"/>
        <source>Show &amp;meaning</source>
        <translation>Visa &amp;betydelse</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Preparing training session</source>
        <translation>Förbereder övningspass</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Preparing training session, please wait...</source>
        <translation>Förbereder övningspass, var god vänta…</translation>
    </message>
    <message>
        <location line="+57"/>
        <source>No matching entries found</source>
        <translation>Inga passande uppslag funna</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unable to find any entry eligible for reading practice. Entries eligible for this training mode are studied vocabulary entries for which all kanji are also studied, and that match the train settings. Please add entries or modify the train settings accordingly if you want to practice this mode.</source>
        <translation>Kan inte hitta något uppslag som passar för läsövning. Passande uppslag är sådana som redan studerats både i ordlistan och som kanji. Lägg till fler uppslag att öva eller ändra övningsinställningarna om du vill kunna använda den här läsövningen.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>There are no more entries to train for the current train settings.</source>
        <translation>Det finns inga fler uppslag att öva med nuvarande inställningar.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No more entries to train</source>
        <translation>Inga fler uppslag att öva</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>&lt;font color=&quot;green&quot;&gt;Correct!&lt;/font&gt;</source>
        <translation>&lt;font color=&quot;green&quot;&gt;Rätt!&lt;/font&gt;</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>&lt;font color=&quot;red&quot;&gt;Error!&lt;/font&gt;</source>
        <translation>&lt;font color=&quot;red&quot;&gt;Fel!&lt;/font&gt;</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>Correct: %1, Wrong: %2, Total: %3</source>
        <translation>Antal rätta svar: %1, fel svar: %2, totalt: %3</translation>
    </message>
    <message>
        <location filename="../src/gui/ReadingTrainer.ui" line="+9"/>
        <source>Type the correct reading for this word:</source>
        <translation>Ange rätt läsning för det här ordet:</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>&amp;Next</source>
        <translation>&amp;Nästa</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>&amp;Ok!</source>
        <translation>&amp;OK!</translation>
    </message>
</context>
<context>
    <name>RelativeDate</name>
    <message>
        <location filename="../src/core/RelativeDate.cc" line="+99"/>
        <source>today</source>
        <translation>idag</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>yesterday</source>
        <translation>igår</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 days ago</source>
        <translation>%1 dagar sedan</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>this week</source>
        <translation>den här veckan</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>last week</source>
        <translation>förra veckan</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 weeks ago</source>
        <translation>%1 veckor sedan</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>this month</source>
        <translation>den här månaden</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>last month</source>
        <translation>förra månaden</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 months ago</source>
        <translation>%1 månader sedan</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>this year</source>
        <translation>det här året</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>last year</source>
        <translation>i år</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 years ago</source>
        <translation>%1 år sedan</translation>
    </message>
</context>
<context>
    <name>RelativeDateEdit</name>
    <message>
        <location filename="../src/gui/RelativeDateEdit.cc" line="+163"/>
        <source>Not set</source>
        <translation>ej angivet</translation>
    </message>
</context>
<context>
    <name>RelativeDatePopup</name>
    <message>
        <location line="-130"/>
        <source>Not set</source>
        <translation>ej angivet</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>days ago</source>
        <translation>dagar sedan</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>weeks ago</source>
        <translation>veckor sedan</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>months ago</source>
        <translation>månader sedan</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>years ago</source>
        <translation>år sedan</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ok</source>
        <translation>Ok</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Relative date:</source>
        <translation>Relativt datum:</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Absolute date:</source>
        <translation>Absolut datum:</translation>
    </message>
</context>
<context>
    <name>ResultsView</name>
    <message>
        <location filename="../src/gui/ResultsView.cc" line="+50"/>
        <source>Select All</source>
        <translation>Markera alla</translation>
    </message>
</context>
<context>
    <name>ResultsViewPreferences</name>
    <message>
        <location filename="../src/gui/ResultsViewPreferences.ui" line="+17"/>
        <source>Results display</source>
        <translation>Resultatvisning</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Smooth scrolling</source>
        <translation>Mjuk rullning</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Results sorting:</source>
        <translation>Sortera resultat:</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>By relevance</source>
        <translation>Efter relevans</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>By score, then relevance</source>
        <translation>Efter poäng, därefter relevans</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Preview</source>
        <translation>Förhandsgranskning</translation>
    </message>
    <message>
        <location filename="../src/gui/PreferencesWindow.cc" line="-187"/>
        <source>Results view</source>
        <translation>Resultatvisning</translation>
    </message>
</context>
<context>
    <name>ResultsViewWidget</name>
    <message>
        <location filename="../src/gui/ResultsViewWidget.cc" line="+43"/>
        <source>Searching...</source>
        <translation>Söker…</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>%1 Results</source>
        <translation>%1 resultat</translation>
    </message>
</context>
<context>
    <name>SavedSearchesOrganizer</name>
    <message>
        <location filename="../src/gui/SavedSearchesOrganizer.ui" line="+14"/>
        <source>Organize saved searches</source>
        <translation>Hantera sparade sökningar</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>You can organize your saved searches here. Double-click to change the search&apos;s label, drag and drop to move or right-click for other options, including deletion.</source>
        <translation>Du kan hantera dina sparade sökningar här. Dubbelklicka för att ändra en söknings namn, drag och släpp för att flytta eller högerklicka för fler alternativ, bland annat ta bort.</translation>
    </message>
</context>
<context>
    <name>SavedSearchesTreeWidget</name>
    <message>
        <location filename="../src/gui/SavedSearchesOrganizer.cc" line="+145"/>
        <source>Delete</source>
        <translation>Ta bort</translation>
    </message>
    <message>
        <location line="+36"/>
        <source>Confirm deletion</source>
        <translation>Bekräfta borttagning</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Are you sure you want to delete the selected search/folder?</source>
        <translation>Vill du verkligen ta bort markerade sökningar och kataloger?</translation>
    </message>
</context>
<context>
    <name>SearchWidget</name>
    <message>
        <location filename="../src/gui/SearchWidget.ui" line="+67"/>
        <source>Previous search</source>
        <translation>Föregående sökning</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Next search</source>
        <translation>Nästa sökning</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>&amp;Reset search...</source>
        <translation>&amp;Nollställ sökning…</translation>
    </message>
</context>
<context>
    <name>ShowUsedInJob</name>
    <message>
        <location filename="../src/gui/kanjidic2/Kanjidic2EntryFormatter.cc" line="+323"/>
        <source>Direct compounds</source>
        <translation>Direkta sammansättningar</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>All compounds</source>
        <translation>Alla sammansättningar</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>(+)</source>
        <translation>(+)</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Make a new search using only this filter</source>
        <translation>Gör en ny sökning med endast det här filtret</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Add this filter to the current search</source>
        <translation>Lägg till det här filtret i nuvarande sökning</translation>
    </message>
</context>
<context>
    <name>ShowUsedInWordsJob</name>
    <message>
        <location line="+10"/>
        <source>Seen in</source>
        <translation>Ingår i</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>All words using this kanji</source>
        <translation>Alla ord som använder detta tecken</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>(+)</source>
        <translation>(+)</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Make a new search using only this filter</source>
        <translation>Gör en ny sökning med endast det här filtret</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Add this filter to the current search</source>
        <translation>Lägg till det här filtret i nuvarande sökning</translation>
    </message>
</context>
<context>
    <name>StudyFilterWidget</name>
    <message>
        <location filename="../src/gui/StudyFilterWidget.cc" line="+30"/>
        <source>Score</source>
        <translation>Poäng</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Min</source>
        <translation>Min</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Max</source>
        <translation>Max</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Study date</source>
        <translation>Övningsdatum</translation>
    </message>
    <message>
        <location line="+24"/>
        <location line="+30"/>
        <location line="+20"/>
        <source>since</source>
        <translation>sedan</translation>
    </message>
    <message>
        <location line="-49"/>
        <location line="+30"/>
        <location line="+20"/>
        <source>until</source>
        <translation>till</translation>
    </message>
    <message>
        <location line="-45"/>
        <source>Last trained</source>
        <translation>Senast övad</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Last mistaken</source>
        <translation>Svarade senast fel</translation>
    </message>
    <message>
        <location line="+114"/>
        <source> since %1</source>
        <translation> sedan %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <location line="+13"/>
        <location line="+10"/>
        <source> until %1</source>
        <translation> till %1</translation>
    </message>
    <message>
        <location line="-20"/>
        <source>, score:[%1-%2]</source>
        <translation>, poäng:[%1–%2]</translation>
    </message>
    <message>
        <location line="-98"/>
        <source>Any</source>
        <translation>Ej specificerad</translation>
    </message>
    <message>
        <location line="+3"/>
        <location line="+85"/>
        <source>Studied</source>
        <translation>Studerad</translation>
    </message>
    <message>
        <location line="-83"/>
        <location line="+116"/>
        <source>Not studied</source>
        <translation>Ej studerad</translation>
    </message>
    <message>
        <location line="-112"/>
        <source>Study status:</source>
        <translation>Övningsstatus:</translation>
    </message>
    <message>
        <location line="+94"/>
        <source>, trained since %1</source>
        <translation>, övad sedan %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>, trained until %1</source>
        <translation>, övad till %1</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>, mistaken since %1</source>
        <translation>, svarat fel sedan %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>, mistaken until %1</source>
        <translation>, svarat fel till %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Study</source>
        <translation>Öva</translation>
    </message>
</context>
<context>
    <name>TagsDialogs</name>
    <message>
        <location filename="../src/gui/TagsDialogs.cc" line="+175"/>
        <source>Set tags for %1</source>
        <translation>Ange taggar för %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Set tags</source>
        <translation>Ange taggar</translation>
    </message>
    <message>
        <location line="+1"/>
        <location line="+35"/>
        <source>these entries:</source>
        <translation>dessa uppslag:</translation>
    </message>
    <message>
        <location line="-35"/>
        <location line="+35"/>
        <source>this entry:</source>
        <translation>det här uppslaget:</translation>
    </message>
    <message>
        <location line="-35"/>
        <source>Enter a space-separated list of tags for </source>
        <translation>Ange taggar separerade med blanksteg för </translation>
    </message>
    <message>
        <location line="+5"/>
        <location line="+35"/>
        <source>Invalid tags</source>
        <translation>Ogiltiga taggar</translation>
    </message>
    <message>
        <location line="-35"/>
        <location line="+35"/>
        <source>&lt;/p&gt;&lt;p&gt;Accepted characters are letters, digits and non-quoting punctuation.&lt;/p&gt;</source>
        <translation>&lt;/p&gt;&lt;p&gt;Tillåtna tecken är bokstäver, siffor samt interpunktion förutom citeringstecken.&lt;/p&gt;</translation>
    </message>
    <message>
        <location line="-35"/>
        <location line="+35"/>
        <source>&lt;p&gt;These tags have invalid characters: </source>
        <translation>&lt;p&gt;De här taggarna innehåller ogiltiga tecken: </translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Add tags to %1</source>
        <translation>Lägg taggar till %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Add tags</source>
        <translation>Lägg till taggar</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Enter a space-separated list of tags to add to </source>
        <translation>Lägg till taggar separerade med blanksteg för </translation>
    </message>
</context>
<context>
    <name>TagsFilterWidget</name>
    <message>
        <location filename="../src/gui/TagsFilterWidget.cc" line="+32"/>
        <source>...</source>
        <translation>…</translation>
    </message>
    <message>
        <location line="+6"/>
        <location line="+62"/>
        <source>Untagged</source>
        <translation>Otaggad</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Tags</source>
        <translation>Taggar</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Tagged %1</source>
        <translation>Taggad %1</translation>
    </message>
</context>
<context>
    <name>TextFilterWidget</name>
    <message>
        <location filename="../src/gui/TextFilterWidget.cc" line="+42"/>
        <source>Romaji search</source>
        <translation>Romajisökning</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>If checked, text typed in romaji will be converted to kana and searched as such.</source>
        <translation>Om ikryssad kommer text inmatad som romaji automatiskt omvandlas till kana innan sökning.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Text search</source>
        <translation>Textsök</translation>
    </message>
</context>
<context>
    <name>TrainSettings</name>
    <message>
        <location filename="../src/gui/TrainSettings.cc" line="+66"/>
        <source>Entries appear totally randomly.</source>
        <translation>Uppslag listas helt slumpmässigt.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Entries with a low score are likely to appear first.</source>
        <translation>Uppslag med låg poäng tenderar att visas först.</translation>
    </message>
    <message>
        <location filename="../src/gui/TrainSettings.ui" line="+14"/>
        <source>Train settings</source>
        <translation>Övningsinställningar</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Date of last training</source>
        <translation>Datum för senaste övningspass</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Before</source>
        <translation>Före</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>After</source>
        <translation>Efter</translation>
    </message>
    <message>
        <location line="+10"/>
        <location line="+7"/>
        <source>PushButton</source>
        <translation>TryckKnapp</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Score</source>
        <translation>Poäng</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>between</source>
        <translation>mellan</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>and</source>
        <translation>och</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Sort bias</source>
        <translation>Sortering</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Bias:</source>
        <translation>Sortera:</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>No bias</source>
        <translation>Ingen sortering</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>By score</source>
        <translation>Efter poäng</translation>
    </message>
</context>
<context>
    <name>YesNoTrainer</name>
    <message>
        <location filename="../src/gui/YesNoTrainer.cc" line="+44"/>
        <source>Answer (&amp;1)</source>
        <translation>Svar (&amp;1)</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Correct! (&amp;2)</source>
        <translation>Rätt! (&amp;2)</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Wrong... (&amp;3)</source>
        <translation>Fel… (&amp;3)</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Skip (&amp;4)</source>
        <translation>Hoppa över (&amp;4)</translation>
    </message>
    <message>
        <location line="+108"/>
        <source>No entries to train</source>
        <translation>Inga uppslag att öva</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>There are no entries to train as of now. Please add entries to your study list or check the train settings.</source>
        <translation>Det finns inga uppslag att öva för tillfället. Lägg uppslag till din studielista eller kontrollera övningsinställningarna.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>All the matching entries have been trained.</source>
        <translation>Alla matchande uppslag har redan övats.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No more entries to train</source>
        <translation>Inga fler uppslag att öva</translation>
    </message>
    <message>
        <location line="-47"/>
        <source>Correct: %1, Wrong: %2, Total: %3</source>
        <translation>Antal rätta svar: %1, fel svar %2, totalt: %3</translation>
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