<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="de" sourcelanguage="en">
<context>
    <name>AboutPage</name>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="17"/>
        <source>About Speaker Gain</source>
        <translation>Über Speaker Gain</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="24"/>
        <source>Version %1</source>
        <translation>Version %1</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="28"/>
        <source>What is going on with the alarm</source>
        <translation>Was beim Wecker passiert</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="35"/>
        <source>The feedback daemon gives the alarm a volume of its own, under the name x-clock-alert-volume, and binds that name to a stored setting called clock.alert.volume. It reads that setting from the general profile — always the general one, whichever profile is active. The system ships it at 100 and offers no way to change it, so the alarm is as loud as the device goes while the ringtone obeys its slider.</source>
        <translation>Der Rückmeldedienst gibt dem Wecker eine eigene Lautstärke unter dem Namen x-clock-alert-volume und bindet diesen Namen an eine gespeicherte Einstellung namens clock.alert.volume. Er liest sie aus dem allgemeinen Profil — immer aus dem allgemeinen, gleich welches Profil gerade aktiv ist. Das System liefert sie mit 100 aus und bietet keine Stelle, sie zu ändern. Deshalb klingelt der Wecker so laut, wie das Gerät kann, während der Klingelton seinem Regler folgt.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="43"/>
        <source>Measured on a Jolla Phone (2026) and an Xperia 10 III; the configuration files are identical on both, so this is how the platform behaves and not a fault of one device.</source>
        <translation>Nachgemessen an einem Jolla Phone (2026) und einem Xperia 10 III; die Konfigurationsdateien sind auf beiden gleich. Es ist also das Verhalten der Plattform und kein Fehler eines einzelnen Geräts.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="46"/>
        <source>What this app changes</source>
        <translation>Was diese App ändert</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="53"/>
        <source>One stored number, in your own settings, through the same interface the system uses itself. No root, nothing patched, no file of the system touched. Set it back to the shipped value and nothing of this app is left behind.</source>
        <translation>Eine gespeicherte Zahl, in deinen eigenen Einstellungen, über dieselbe Schnittstelle, die das System selbst benutzt. Kein Root, nichts gepatcht, keine Systemdatei angefasst. Stell den Auslieferungswert wieder her, und von dieser App bleibt nichts zurück.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="57"/>
        <source>What runs in the background</source>
        <translation>Was im Hintergrund läuft</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="64"/>
        <source>Nothing, once the app is closed. The system can store a volume for an output only while that output plays, so a value set for another one is kept by the app. While the app is open — in front or as a cover on the home screen — it listens to the sound system for that output to come into use and then sets the value. It listens only while a value waits. Closed, nothing of it runs; the value waits until the app is next open while that output plays.</source>
        <translation>Nichts, sobald die App geschlossen ist. Das System kann eine Lautstärke für einen Ausgang nur ablegen, während dieser Ausgang spielt. Einen Wert für einen anderen Ausgang merkt sich deshalb die App. Solange sie offen ist — im Vordergrund oder als Cover auf dem Startbildschirm —, horcht sie beim Tonsystem darauf, dass dieser Ausgang benutzt wird, und setzt dann den Wert. Sie horcht nur, solange ein Wert wartet. Ist die App geschlossen, läuft nichts davon; der Wert wartet, bis die App das nächste Mal offen ist, während dieser Ausgang spielt.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="72"/>
        <source>Versions up to 0.2.1 used a service of the system for this (harbour-speakergain-apply), which kept running until the value was set, also across restarts of the phone. The update ends it, and the app removes what started it.</source>
        <translation>Versionen bis 0.2.1 haben dafür einen Dienst des Systems benutzt (harbour-speakergain-apply), der lief, bis der Wert gesetzt war, auch über Neustarts des Telefons hinweg. Das Update beendet ihn, und die App entfernt, was ihn gestartet hat.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="75"/>
        <source>What runs with system rights</source>
        <translation>Was mit Systemrechten läuft</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="82"/>
        <source>One helper, for USB headphones whose volume control reports a scale the system cannot use, such as the Bose Noise Cancelling Wired Earbuds: whatever is set, they stay at full volume. The package brings a sound profile that has the phone set their volume in software, and a list of the headphones known to need it. Headphones on that list need nothing more, and nothing runs with system rights for them.</source>
        <translation>Ein Helfer, für USB-Kopfhörer, deren Lautstärkeregler eine Skala meldet, mit der das System nichts anfangen kann, etwa die Bose Noise Cancelling Wired Earbuds: Egal, was eingestellt ist, sie bleiben bei voller Lautstärke. Das Paket bringt ein Klangprofil mit, bei dem das Telefon ihre Lautstärke per Software regelt, und eine Liste der Kopfhörer, die es bekanntermaßen brauchen. Kopfhörer auf dieser Liste brauchen nichts weiter, und für sie läuft nichts mit Systemrechten.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="89"/>
        <source>Other headphones with that fault are offered on the outputs page. Only when you switch one on there does the helper run, as the service harbour-speakergain-softvol@, for a few seconds. It does three things and ends: it writes the device&apos;s USB vendor and product id into /etc/udev/rules.d/91-harbour-speakergain-softvol-local.rules, has the system read its device rules again, and disconnects and reconnects that one USB device so the sound system takes it up with the new profile. Switching it off takes the id out again.</source>
        <translation>Andere Kopfhörer mit diesem Fehler bietet die Seite der Ausgaben an. Nur wenn du dort einen einschaltest, läuft der Helfer, als Dienst harbour-speakergain-softvol@, für ein paar Sekunden. Er tut drei Dinge und beendet sich: Er schreibt die USB-Hersteller- und Produktnummer des Geräts in /etc/udev/rules.d/91-harbour-speakergain-softvol-local.rules, lässt das System seine Geräteregeln neu lesen und trennt dieses eine USB-Gerät kurz und verbindet es wieder, damit das Tonsystem es mit dem neuen Profil übernimmt. Ausschalten nimmt die Nummer wieder heraus.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="97"/>
        <source>Starting a service of the system would normally ask for the device password. A rule the package brings (polkit) lets your user start exactly this one without it, and only as &quot;add&quot; or &quot;remove&quot; with a four-digit vendor and product id; the helper refuses anything else. It never starts by itself, never stays, and touches nothing but that one file and that one device. Removing the app removes the service, the rule and the list.</source>
        <translation>Einen Dienst des Systems zu starten, würde normalerweise nach dem Gerätepasswort fragen. Eine Regel, die das Paket mitbringt (polkit), erlaubt deinem Benutzer, genau diesen einen ohne Passwort zu starten, und nur als „add“ oder „remove“ mit einer vierstelligen Hersteller- und Produktnummer; alles andere lehnt der Helfer ab. Er startet nie von selbst, bleibt nie, und er fasst nichts an außer dieser einen Datei und diesem einen Gerät. Wird die App entfernt, verschwinden Dienst, Regel und Liste mit ihr.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="100"/>
        <source>Language</source>
        <translation>Sprache</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="109"/>
        <source>Show this app in English</source>
        <translation>Diese App auf Englisch zeigen</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="110"/>
        <source>Regardless of the language the phone is set to. Takes effect the next time the app is started, and changes nothing outside this app.</source>
        <translation>Unabhängig davon, worauf das Telefon eingestellt ist. Wirkt beim nächsten Start der App und verändert nichts außerhalb dieser App.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="114"/>
        <source>When something does not work</source>
        <translation>Wenn etwas nicht funktioniert</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="121"/>
        <source>If a slider moves but the sound does not change, a short report of how this phone&apos;s sound is put together helps to find out why. It holds no serial numbers, addresses or names, and you see all of it before you pass it on.</source>
        <translation>Wenn sich ein Regler bewegt, der Ton sich aber nicht ändert, hilft ein kurzer Bericht darüber, wie der Ton dieses Telefons aufgebaut ist, die Ursache zu finden. Er enthält keine Seriennummern, Adressen oder Namen, und du siehst ihn vollständig, bevor du ihn weitergibst.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="125"/>
        <source>Diagnostics</source>
        <translation>Diagnose</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="129"/>
        <source>Licence</source>
        <translation>Lizenz</translation>
    </message>
    <message>
        <location filename="../qml/pages/AboutPage.qml" line="137"/>
        <source>GPL-3.0-or-later. No warranty — but the whole change is one number in your own settings, and the button next to the slider puts the shipped value back.</source>
        <translation>GPL-3.0-or-later. Ohne Gewähr — die ganze Änderung ist eine Zahl in deinen eigenen Einstellungen, und der Knopf neben dem Regler holt den Auslieferungswert zurück.</translation>
    </message>
</context>
<context>
    <name>AlarmView</name>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="22"/>
        <source>Set to %1 % and read back.</source>
        <translation>Auf %1 % gesetzt und gegengelesen.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="24"/>
        <source>Could not set the value.</source>
        <translation>Der Wert ließ sich nicht setzen.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="37"/>
        <source>About</source>
        <translation>Über</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="48"/>
        <source>Speaker Gain</source>
        <translation>Speaker Gain</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="49"/>
        <source>The output volumes the system keeps to itself</source>
        <translation>Die Ausgabelautstärken, die das System für sich behält</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="59"/>
        <source>profiled is not answering. Without it this page cannot read or write anything.</source>
        <translation>profiled antwortet nicht. Ohne ihn kann diese Seite weder lesen noch schreiben.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="62"/>
        <source>Alarm</source>
        <translation>Wecker</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="70"/>
        <source>The alarm has a loudness of its own. It is not the ringtone volume, the volume keys do not reach it, and switching to Silent leaves it untouched — by design, because an alarm is meant to wake you. What is missing is a way to set it, and this is that way.</source>
        <translation>Der Wecker hat eine eigene Lautstärke. Sie ist nicht die des Klingeltons, die Lautstärketasten erreichen sie nicht, und das Profil „Lautlos“ lässt sie unangetastet — gewollt, denn ein Wecker soll wecken. Was fehlt, ist eine Stelle, sie einzustellen. Das ist diese Stelle.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="82"/>
        <source>Alarm volume</source>
        <translation>Weckerlautstärke</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="87"/>
        <source>Shipped default</source>
        <translation>Auslieferungswert</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="90"/>
        <location filename="../qml/pages/AlarmView.qml" line="95"/>
        <location filename="../qml/pages/AlarmView.qml" line="99"/>
        <location filename="../qml/pages/AlarmView.qml" line="135"/>
        <source>unknown</source>
        <translation>unbekannt</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="94"/>
        <source>Ringtone, for comparison</source>
        <translation>Klingelton, zum Vergleich</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="98"/>
        <source>Active profile</source>
        <translation>Aktives Profil</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="118"/>
        <source>Every alarm fades in over its first 20 seconds, from silence up to the value above. So this is the ceiling of that ramp, not the volume it starts at.</source>
        <translation>Jeder Wecker blendet über seine ersten 20 Sekunden auf, von still bis zum Wert oben. Das ist also die Obergrenze dieser Rampe, nicht die Lautstärke beim Einsetzen.</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="123"/>
        <source>Back to the default</source>
        <translation>Zurück auf den Auslieferungswert</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="129"/>
        <source>Does this value survive?</source>
        <translation>Bleibt dieser Wert erhalten?</translation>
    </message>
    <message>
        <location filename="../qml/pages/AlarmView.qml" line="161"/>
        <source>Written straight away, but the file behind it is only flushed a few seconds later. If the phone is cut from power right after a change, the change is the thing that is lost.</source>
        <translation>Sofort geschrieben, aber die Datei dahinter wird erst ein paar Sekunden später festgeschrieben. Wer das Gerät direkt nach einer Änderung hart vom Strom nimmt, verliert genau diese Änderung.</translation>
    </message>
</context>
<context>
    <name>CoverPage</name>
    <message>
        <location filename="../qml/cover/CoverPage.qml" line="13"/>
        <source>Alarm</source>
        <translation>Wecker</translation>
    </message>
</context>
<context>
    <name>DiagnosticsPage</name>
    <message>
        <location filename="../qml/pages/DiagnosticsPage.qml" line="24"/>
        <source>Save to Documents</source>
        <translation>In Dokumente speichern</translation>
    </message>
    <message>
        <location filename="../qml/pages/DiagnosticsPage.qml" line="27"/>
        <source>Saved as %1</source>
        <translation>Gespeichert als %1</translation>
    </message>
    <message>
        <location filename="../qml/pages/DiagnosticsPage.qml" line="28"/>
        <source>Could not save the report.</source>
        <translation>Der Bericht konnte nicht gespeichert werden.</translation>
    </message>
    <message>
        <location filename="../qml/pages/DiagnosticsPage.qml" line="32"/>
        <source>Copy to clipboard</source>
        <translation>In die Zwischenablage kopieren</translation>
    </message>
    <message>
        <location filename="../qml/pages/DiagnosticsPage.qml" line="35"/>
        <source>Copied. Paste it into a message or an issue.</source>
        <translation>Kopiert. Füge ihn in eine Nachricht oder ein Issue ein.</translation>
    </message>
    <message>
        <location filename="../qml/pages/DiagnosticsPage.qml" line="39"/>
        <source>Collect again</source>
        <translation>Neu erfassen</translation>
    </message>
    <message>
        <location filename="../qml/pages/DiagnosticsPage.qml" line="49"/>
        <source>Diagnostics</source>
        <translation>Diagnose</translation>
    </message>
    <message>
        <location filename="../qml/pages/DiagnosticsPage.qml" line="56"/>
        <source>Plug in the headphones that misbehave and start some music before opening this page, then pull down to copy or save. The report describes how this phone&apos;s sound is wired up and names what looks wrong. It is not sent anywhere: you decide where it goes.</source>
        <translation>Stecke die Kopfhörer an, die Probleme machen, und starte Musik, bevor du diese Seite öffnest; dann zum Kopieren oder Speichern nach unten ziehen. Der Bericht beschreibt, wie der Ton dieses Telefons verschaltet ist, und benennt, was auffällt. Er wird nirgendwohin geschickt: Du entscheidest, wo er landet.</translation>
    </message>
    <message>
        <location filename="../qml/pages/DiagnosticsPage.qml" line="64"/>
        <source>Left out or masked: serial numbers, Bluetooth addresses, the names given to Bluetooth devices, the phone&apos;s name and which apps are playing.</source>
        <translation>Weggelassen oder unkenntlich: Seriennummern, Bluetooth-Adressen, die Namen, die Bluetooth-Geräten gegeben wurden, der Name des Telefons und welche Apps gerade spielen.</translation>
    </message>
</context>
<context>
    <name>OutputsView</name>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="45"/>
        <source>Set to %1 %. The database behind it is written a few seconds later.</source>
        <translation>Auf %1 % gesetzt. Die Datenbank dahinter wird ein paar Sekunden später geschrieben.</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="43"/>
        <location filename="../qml/pages/OutputsView.qml" line="60"/>
        <location filename="../qml/pages/OutputsView.qml" line="73"/>
        <source>Could not set the value.</source>
        <translation>Der Wert ließ sich nicht setzen.</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="59"/>
        <source>Set to %1 %. It holds while the output stays connected.</source>
        <translation>Auf %1 % gesetzt. Das gilt, solange der Ausgang verbunden bleibt.</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="71"/>
        <source>The volume of these headphones is now set in software. They were connected anew for it.</source>
        <translation>Die Lautstärke dieser Kopfhörer wird jetzt per Software geregelt. Dafür wurden sie neu verbunden.</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="72"/>
        <source>Back to the device&apos;s own volume control. The headphones were connected anew for it.</source>
        <translation>Wieder der eigene Lautstärkeregler des Geräts. Dafür wurden die Kopfhörer neu verbunden.</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="126"/>
        <source>Per output</source>
        <translation>Je Ausgang</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="128"/>
        <source>Playing on: %1</source>
        <translation>Spielt gerade auf: %1</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="129"/>
        <source>No output could be identified</source>
        <translation>Kein Ausgang erkennbar</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="138"/>
        <source>Every output carries a volume of its own, per kind of sound, and the system restores it when the output changes. The output in use takes a new value at once. The system files no other, so for those the value is kept here and set once that output is in use while the app is open, in front or as a cover. Nothing of it runs once the app is closed.</source>
        <translation>Jeder Ausgang hat eine eigene Lautstärke, je Art von Ton, und das System holt sie beim Wechsel des Ausgangs wieder hervor. Der Ausgang, der gerade benutzt wird, übernimmt einen neuen Wert sofort. Für alle anderen legt das System nichts ab — dort merkt sich die App den Wert und setzt ihn, sobald der Ausgang benutzt wird, während die App offen ist, im Vordergrund oder als Cover. Ist die App geschlossen, läuft nichts davon.</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="190"/>
        <source>Waiting: %1 % once this output is in use while the app is open.</source>
        <translation>Vorgemerkt: %1 %, sobald dieser Ausgang benutzt wird, während die App offen ist.</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="211"/>
        <source>On Bluetooth the phone takes over the headphones&apos; own volume control and holds it at full scale, so what you hear is set by the value above. The headphones&apos; own volume further down can be lowered as well — it stays where the phone left it after disconnecting.</source>
        <translation>Über Bluetooth übernimmt das Telefon die Lautstärkeregelung des Kopfhörers und hält sie auf Anschlag, also bestimmt der Wert oben, was du hörst. Die eigene Lautstärke des Kopfhörers weiter unten lässt sich zusätzlich senken — sie bleibt nach dem Trennen dort, wo das Telefon sie gelassen hat.</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="215"/>
        <source>The output&apos;s own volume</source>
        <translation>Lautstärke des Ausgangs selbst</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="225"/>
        <source>Beneath the values above, Bluetooth headphones and USB sound cards have a volume of their own, and it starts at full scale. On Bluetooth the figure is sent to the headphones and becomes their own volume; on USB it is the card&apos;s mixer. The system stores none of these: a value holds while the output stays connected, and the next connection may start at full scale again.</source>
        <translation>Unter den Werten oben haben Bluetooth-Kopfhörer und USB-Soundkarten eine eigene Lautstärke, und die steht anfangs auf Anschlag. Bei Bluetooth wird die Zahl an den Kopfhörer geschickt und wird zu seiner eigenen Lautstärke; bei USB ist es der Mixer der Karte. Das System speichert davon nichts: Ein Wert gilt, solange der Ausgang verbunden bleibt, und die nächste Verbindung kann wieder auf Anschlag beginnen.</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="244"/>
        <source>Bluetooth</source>
        <translation>Bluetooth</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="245"/>
        <source>USB</source>
        <translation>USB</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="255"/>
        <source>These headphones report a volume scale the system cannot use: whatever is set, they stay at full volume. Speaker Gain can have the phone set their volume in software instead. They are connected anew for it, which takes a few seconds.</source>
        <translation>Diese Kopfhörer melden eine Lautstärkeskala, mit der das System nichts anfangen kann: Was auch eingestellt wird, sie bleiben auf voller Lautstärke. Speaker Gain kann das Telefon ihre Lautstärke stattdessen per Software regeln lassen. Dafür werden sie neu verbunden, das dauert ein paar Sekunden.</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="260"/>
        <source>Set volume in software</source>
        <translation>Lautstärke per Software regeln</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="270"/>
        <source>The volume of these headphones is set in software, because their own volume control reports a scale the system cannot use.</source>
        <translation>Die Lautstärke dieser Kopfhörer wird per Software geregelt, weil ihr eigener Regler eine Skala meldet, mit der das System nichts anfangen kann.</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="275"/>
        <source>Use the device&apos;s own control again</source>
        <translation>Wieder den Regler des Geräts nutzen</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="295"/>
        <source>Fixed volumes of the policy</source>
        <translation>Feste Lautstärken der Audio-Policy</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="306"/>
        <source>not stored yet</source>
        <translation>noch nicht gespeichert</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="282"/>
        <source>What the headphones themselves decide</source>
        <translation>Was der Kopfhörer selbst entscheidet</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="46"/>
        <source>Kept at %1 %. It is set once this output is in use while the app is open.</source>
        <translation>Auf %1 % vorgemerkt. Der Wert wird gesetzt, sobald dieser Ausgang benutzt wird, während die App offen ist.</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="115"/>
        <source>About</source>
        <translation>Über</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="191"/>
        <source>Nothing stored yet; this is the figure the system starts this output from.</source>
        <translation>Noch nichts gespeichert; das ist der Wert, mit dem das System diesen Ausgang startet.</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="196"/>
        <source>Forget</source>
        <translation>Verwerfen</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="292"/>
        <source>Bluetooth headphones carry settings no audio system can reach, because they live in the headphones and not in the phone: the listening modes — noise cancelling and ambient sound — and the charge of their battery. For Sony models there is a client for Sailfish OS, called Lauscher, that speaks their own protocol and offers exactly those. It is worth knowing about here because ambient sound changes how loud the world around you is, which is half of what people mean by too loud. Setting a volume is not among its functions at the time of writing, so the slider above stays the only way to make the music itself quieter.</source>
        <translation>Bluetooth-Kopfhörer tragen Einstellungen, an die kein Audiosystem herankommt, weil sie im Kopfhörer sitzen und nicht im Telefon: die Hörmodi — Geräuschunterdrückung und Umgebungston — und der Ladestand ihres Akkus. Für Sony-Modelle gibt es unter Sailfish OS einen Klienten namens Lauscher, der deren eigenes Protokoll spricht und genau das anbietet. Hier ist er erwähnenswert, weil der Umgebungston bestimmt, wie laut die Welt um dich herum ist — und das ist die Hälfte dessen, was Leute mit „zu laut“ meinen. Eine Lautstärke zu setzen gehört zum jetzigen Zeitpunkt nicht zu seinen Funktionen; der Regler oben bleibt also der einzige Weg, die Musik selbst leiser zu machen.</translation>
    </message>
    <message>
        <location filename="../qml/pages/OutputsView.qml" line="160"/>
        <location filename="../qml/pages/OutputsView.qml" line="305"/>
        <source>in use</source>
        <translation>in Benutzung</translation>
    </message>
</context>
<context>
    <name>SpeakerGain</name>
    <message>
        <location filename="../src/speakergain.cpp" line="155"/>
        <location filename="../src/speakergain.cpp" line="186"/>
        <source>profiled is not answering on the session bus.</source>
        <translation>profiled antwortet nicht auf dem Sitzungsbus.</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="164"/>
        <source>profiled refused the value.</source>
        <translation>profiled hat den Wert abgelehnt.</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="209"/>
        <source>Written, but reading it back gave a different value.</source>
        <translation>Geschrieben, aber beim Gegenlesen kam ein anderer Wert zurück.</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="262"/>
        <source>Survives updates</source>
        <translation>Bleibt bei Updates erhalten</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="264"/>
        <source>The value is in your home directory and belongs to no package. A restart keeps it, and so does a system update.</source>
        <translation>Der Wert liegt in deinem Benutzerverzeichnis und gehört keinem Paket. Ein Neustart lässt ihn stehen, ein Systemupdate ebenso.</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="274"/>
        <source>No package owns this file</source>
        <translation>Kein Paket besitzt diese Datei</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="276"/>
        <source>Nothing in the package database claims this path, so no update is going to replace it. Whoever put it there is not the package manager.</source>
        <translation>Kein Eintrag der Paketdatenbank beansprucht diesen Pfad, also ersetzt ihn auch kein Update. Wer die Datei dort abgelegt hat, war nicht die Paketverwaltung.</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="291"/>
        <source>Kept as configuration</source>
        <translation>Als Konfiguration erhalten</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="293"/>
        <source>The file belongs to %1 and is marked as configuration. An update keeps your edit and puts its own version next to it.</source>
        <translation>Die Datei gehört zu %1 und ist als Konfiguration gekennzeichnet. Ein Update lässt deine Änderung stehen und legt seine eigene Fassung daneben.</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="297"/>
        <source>An update throws this away</source>
        <translation>Ein Update verwirft das hier</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="299"/>
        <source>The file belongs to %1 and is not marked as configuration. The next update of that package overwrites it — no backup copy, no notice.</source>
        <translation>Die Datei gehört zu %1 und ist nicht als Konfiguration gekennzeichnet. Das nächste Update dieses Pakets überschreibt sie — ohne Sicherungskopie, ohne Hinweis.</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="490"/>
        <source>Media</source>
        <translation>Medien</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="492"/>
        <source>Calls</source>
        <translation>Telefonie</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="494"/>
        <source>VoIP</source>
        <translation>VoIP</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="501"/>
        <source>Speaker</source>
        <translation>Lautsprecher</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="503"/>
        <source>Wired headphones</source>
        <translation>Kabelkopfhörer</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="505"/>
        <location filename="../src/speakergain.cpp" line="524"/>
        <source>Wired headset</source>
        <translation>Kabel-Headset</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="507"/>
        <source>Bluetooth, music</source>
        <translation>Bluetooth, Musik</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="509"/>
        <source>Bluetooth, calls (HFP)</source>
        <translation>Bluetooth, Telefonie (HFP)</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="511"/>
        <source>Bluetooth, calls (HSP)</source>
        <translation>Bluetooth, Telefonie (HSP)</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="513"/>
        <source>Bluetooth, calls</source>
        <translation>Bluetooth, Telefonie</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="515"/>
        <location filename="../src/speakergain.cpp" line="526"/>
        <source>Line out</source>
        <translation>Line-Ausgang</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="517"/>
        <source>USB audio</source>
        <translation>USB-Audio</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="529"/>
        <source>Speaker and %1</source>
        <translation>Lautsprecher und %1</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="858"/>
        <source>PulseAudio could not be reached.</source>
        <translation>PulseAudio war nicht erreichbar.</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="874"/>
        <source>PulseAudio refused the connection: %1</source>
        <translation>PulseAudio hat die Verbindung abgelehnt: %1</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="886"/>
        <source>PulseAudio dropped the connection: %1</source>
        <translation>PulseAudio hat die Verbindung fallen lassen: %1</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="909"/>
        <location filename="../src/speakergain.cpp" line="920"/>
        <source>The stream could not be opened: %1</source>
        <translation>Der Stream ließ sich nicht öffnen: %1</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="945"/>
        <location filename="../src/speakergain.cpp" line="1063"/>
        <source>PulseAudio did not accept the volume.</source>
        <translation>PulseAudio hat die Lautstärke nicht angenommen.</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="1055"/>
        <source>Set, but the output reports %1 %.</source>
        <translation>Gesetzt, aber der Ausgang meldet %1 %.</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="1061"/>
        <source>The output is gone.</source>
        <translation>Der Ausgang ist weg.</translation>
    </message>
    <message>
        <location filename="../src/speakergain.cpp" line="1227"/>
        <source>The value could not be kept.</source>
        <translation>Der Wert ließ sich nicht vormerken.</translation>
    </message>
    <message>
        <location filename="../src/softvol.cpp" line="177"/>
        <source>Not a USB device id: %1</source>
        <translation>Keine USB-Gerätekennung: %1</translation>
    </message>
    <message>
        <location filename="../src/softvol.cpp" line="189"/>
        <source>The helper did not run (systemctl: %1).</source>
        <translation>Der Helfer ist nicht gelaufen (systemctl: %1).</translation>
    </message>
</context>
<context>
    <name>StepsView</name>
    <message>
        <location filename="../qml/pages/StepsView.qml" line="24"/>
        <source>not listed</source>
        <translation>nicht aufgeführt</translation>
    </message>
    <message>
        <location filename="../qml/pages/StepsView.qml" line="26"/>
        <source>%1 steps  ·  down to %2 dB  ·  biggest jump %3 dB</source>
        <extracomment>%1 step count, %2 quietest step in dB, %3 largest jump between two steps</extracomment>
        <translation>%1 Stufen  ·  hinunter bis %2 dB  ·  größter Sprung %3 dB</translation>
    </message>
    <message>
        <location filename="../qml/pages/StepsView.qml" line="36"/>
        <source>About</source>
        <translation>Über</translation>
    </message>
    <message>
        <location filename="../qml/pages/StepsView.qml" line="47"/>
        <source>Volume steps</source>
        <translation>Lautstärkestufen</translation>
    </message>
    <message>
        <location filename="../qml/pages/StepsView.qml" line="48"/>
        <source>What one key press does, per output</source>
        <translation>Was ein Tastendruck bewirkt, je Ausgang</translation>
    </message>
    <message>
        <location filename="../qml/pages/StepsView.qml" line="57"/>
        <source>The volume keys do not move a percentage, they walk through a list of fixed levels, and every output has a list of its own — one for calls, one for VoIP, one for everything else. A short list means coarse steps. The biggest jump is the largest gap between two neighbouring levels, which is what a single press costs you at its worst point.</source>
        <translation>Die Lautstärketasten verschieben keinen Prozentwert, sie laufen durch eine Liste fester Pegel — und jeder Ausgang hat eigene Listen: eine fürs Telefonieren, eine für VoIP, eine für alles Übrige. Eine kurze Liste heißt grobe Schritte. Der größte Sprung ist der weiteste Abstand zwischen zwei benachbarten Pegeln, also das, was ein einzelner Tastendruck an seiner gröbsten Stelle kostet.</translation>
    </message>
    <message>
        <location filename="../qml/pages/StepsView.qml" line="75"/>
        <source>Used by: %1</source>
        <translation>Benutzt von: %1</translation>
    </message>
    <message>
        <location filename="../qml/pages/StepsView.qml" line="79"/>
        <source>Calls</source>
        <translation>Telefonie</translation>
    </message>
    <message>
        <location filename="../qml/pages/StepsView.qml" line="83"/>
        <source>VoIP</source>
        <translation>VoIP</translation>
    </message>
    <message>
        <location filename="../qml/pages/StepsView.qml" line="87"/>
        <source>Everything else</source>
        <translation>Alles Übrige</translation>
    </message>
    <message>
        <location filename="../qml/pages/StepsView.qml" line="92"/>
        <source>Loudness warning above step</source>
        <translation>Lautstärkewarnung ab Stufe</translation>
    </message>
    <message>
        <location filename="../qml/pages/StepsView.qml" line="98"/>
        <source>Why there is no slider here</source>
        <translation>Warum es hier keinen Regler gibt</translation>
    </message>
    <message>
        <location filename="../qml/pages/StepsView.qml" line="114"/>
        <source>Changing these tables needs root, and the change would be gone again after the next update of the audio adaptation. This app therefore reads them and leaves them alone.</source>
        <translation>Diese Tabellen zu ändern braucht Root, und die Änderung wäre nach dem nächsten Update der Audio-Anpassung wieder weg. Diese App liest sie deshalb und lässt sie in Ruhe.</translation>
    </message>
</context>
</TS>
