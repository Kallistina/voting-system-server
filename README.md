wget http://rhodes.mm.di.uoa.gr/SysPro/HW2/political_parties.txt

./create_input.sh political_parties.txt 100

./tallyVotes.sh tallyResultsFile
./poller 5634 8 16 poll-log pollStats.txt

./pollSwayer localhost 5634 inputFile

./processLogFile.sh pollerResultsFile

diff tallyResultsFile pollStats.txt

diff pollerResultsFile pollStats.txt












Προγραμματισμός Συστήματος
Εργασία 2 - Εαρινό Εξάμηνο 2023

Ονοματεπώνυμο: Καλλιόπη - Χριστίνα Δεσποτίδου
Α.Μ.: 11152020000045


Compile:
            make


Στο αρχείο headFile.h βρίσκονται όλες οι βιβλιοθήκες, οι ορισμοί, οι συναρτήσεις και τα stucts που χρησιμοποιώ.

Problem 1: Multi-threaded network server

Αρχεία:
poller.cpp
headFile.cpp

Run:
        ./poller 5634 8 16 pollLog.txt pollStats.txt

main: H συνάρτηση terminate() καλείται οποτεδήποτε πατηθεί control C για να αποθηκεύσει όση πληροφορία έχει μέχρι στιγμής και να τερματίσει το πρόγραμμα(αναλύεται στη συνέχεια).
Διαβάζει και φροντίζει τα ορίσματα από τη γραμμή εντολών και ανοιγεί τα δύο αρχεία στα οποία θα πρέπει να καταγραφεί η πληροφορία των εκλογών.
Δημιουργεί όσα νήματα-εργατών ζήτηθηκαν σύμφωνα με το numWorkerthreads.
Αρχίζει να στήνει τον server μέσω των bind(), listen(), accept().
Με την accept() παίρνει τον καινούριο αριθμό του socket και το αποθηκεύει στο buffer.
Αν γεμίσει ο buffer, περιμένει και διαχειρίζεται τους δείκτες του.

thread_f: Όσο ο buffer είναι άδειος, περιμένει. Διαχειρίζεται τους δείκτες του buffer και παίρνει πάντα socket από εκεί που δείχνει ο b_start index.
To πρώτο μήνυμα που στέλνει είναι το "SEND NAME PLEASE" για να ζητήσει το όνομα του ψηφοφόρου μέσω της write().
Μετά διαβάζει το δεύτερο μήνυμα, δηλαδή το όνομα του ψηφοφόρου που ζήτησε πριν από το socket μέσω της read() (διαβάζει ένα ένα τα bytes μέχρι να βρει τον χαρατήρα $ που έχουμε περάσει στο τέλος του μηνύματος στην αντίστοιχη συνάρτηση του pollSwayer για να ξέρει πότε τελειώσε το μήνυμα).
Αποθηκεύει το όνομα και το επώνυμο σε ένα string name και μετά ελέγχει αν υπάρχει αυτό το όνομα ήδη στο vector voters μέσω της find() και ενημερώνει το flag already_voted, αν το βρει.
Αν το βρει λοιπόν, πρέπει να γράψει το μήνυμα ALREADY VOTED και προχωράει στον επόμενο ψηφοφόρο.
Αν δεν το βρει, αποθηκεύει το όνομα του καινούριου ψηφοφόρου στο vector.
Συνεχίζει γράφoντας το μήνυμα SEND VOTE PLEASE για να ζητήσει την ψήφο που αντιστοιχεί στον συγκεκριμένο ψηφοφόρο.
Μετά την διαβάζει με τον τρόπο που περιγράφηκε και πριν σχετικά με το πώς διαβάζει το όνομα.
Την αποθηκεύει προσωρινά σε ένα string political party.
Στέλνει το μήνυμα για να δείξει ότι καταγράφηκε η ψήφος.
Ελέγχει αν έχει ήδη το συγκεκριμένο political party στο vector political_parties, για να μην το ξανααποθηκεύσει. Αν το βρει, ενημερώνει τις ψήφους του, αλλιώς το αποθηκεύει και αρχικοποιεί τις ψήφους σε 1.

terminate: Καλείται όταν διακόπτεται το πρόγραμμα πατώντας το control C.
Φροντίζει να ταξινομήσει πρώτα τα κόμματα ανάλογα με το ποιο έχει μαζέψει τις περισσότερες ψήφους.
Κάνοντας iterate όλο το vector political_parties βρίσκει το κόμμα με τις περισσότερες ψήφους και αποθηκεύει τον δείκτη που δείχνει σ'αυτό. Μετά το περνάει, δηλαδή περνάει το όνομα του κόμματος και τις ψήφους, στο αρχείο poll_stats και το διαγράφει οριστικά από το vector political_parties. Αυτό επαναλαμβάνεται μέχρι να μην μείνει κανένα κόμμα.
Τέλος, τερματίζει όλο το πρόγραμμα.


Problem 2: Batch client for testing purposes 

Αρχεία:
pollSwayer.cpp
headFile.cpp

Run:
        ./pollSwayer linux01.di.uoa.gr 5634 inputFile.txt


main: Αρχικά, ανοίγει και διαβάζει από το αρχείο inputFile.txt.
Αποθηκεύει κάθε γραμμή του αρχείου στο vector fileLines και δημιουργεί ένα thread για κάθε γραμμή. Αποθηκεύει το id κάθε thread στο vector threadIds.
Περιμένει να τελειώσουν όλα τα threads πριν τερματίσει το πρόγραμμα με τη βοήθεια της thread_join().

thread_f: Αρχικά, κάνει parse το πρώτο στοιχείο του vector fileLines, δηλαδή κάθε φορά τη νέα γραμμή με τα στοιχεία του και την ψήφο του ψηφοφόρου. Τα αποθηκεύει στο struct voter όπου κρατούνται ξεχωριστά το όνομα, το επίθετο και το κόμμα που ψήφισε ο ψηφοφόρος.
Συνδέεται με τον server για να ξεκινήσει η επικοινωνία μέσω socket(), gethostbyname(), connect().
Διαβάζει από τον poller το μήνυμα "SEND NAME PLEASE" και το αποθηκεύει στη  message_s.
Ετοιμάζει στη voter_name τα στοιχεία που πρέπει να στείλει και μετά απαντάει στον poller μέσω της write() και του socket.
Αν το επόμενο μήνυμα που θα διαβάει είναι ALREADY VOTED, κλείνει το socket και τερματίζει το thread.
Αν το μήνυμα είναι "SEND VOTE PLEASE", φροντίζει να στείλει το political_party μέσω της write(), κλείνει το socket και τερμαρίζει το thread.

                                                                                    
Problem 3: Bash scripts 
3.1)
Run:
        ./create_input.sh politicalParties.txt numLines

Αρχεία:
Το politicalParties.txt που μας δόθηκε
create_input.sh : Ελέγχει τα ορίσματα, δημιουργεί το inputFile και το γεμίζει.
inputFile

3.2)
Run:
        ./tallyVotes.sh tallyResultsFile

Αρχεία:
inputFile (δημιουργήθηκε στο προηγούμενο υποερώτημα)
tallyVotes.sh
inputFile_noDuplicates
tallyResultsFile_unsorted
tallyResultsFile

tallyVotes.sh: Ελέγχει αν υπάρχει inputFile και αν μπορούμε να διβάσουμε από αυτό.
Δημιουργεί το inputFile_noDuplicates που είναι το inputFile χωρίς διπλότυπα.
Αποθηκεύει στο αρχείο tallyResultsFile_unsorted τα αποτελέσματα των εκλογών σύμφωνα με το inputFile_noDuplicates.
Ύστερα ταξινομεί το tallyResultsFile_unsorted και τα αποθηκεύει στο tallyResultsFile.
Διαγράφει τα αρχεία tallyResultsFile_unsorted, inputFile_noDuplicates γιατί δεν μας χρειάζονται.


3.3)
Run:
        ./processLogFile.sh poll-log

Αρχεία:
poll-log 
processLogFile.sh
poll-log_noDuplicates
pollerResultsFile 
pollerResultsFile_unsorted

processLogFile.sh: Ελέγχει αν υπάρχει poll-log και αν μπορούμε να διβάσουμε από αυτό.
Δημιουργεί το poll-log_noDuplicates που είναι το poll-log χωρίς διπλότυπα.
Αποθηκεύει στο αρχείο pollerResultsFile_unsorted τα αποτελέσματα των εκλογών σύμφωνα με το poll-log_noDuplicates.
Ύστερα ταξινομεί το pollerResultsFile_unsorted και τα αποθηκεύει στο pollerResultsFile.
Διαγράφει τα αρχεία pollerResultsFile_unsorted, poll-log_noDuplicates γιατί δεν μας χρειάζονται.