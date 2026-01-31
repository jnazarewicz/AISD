#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <bitset>
#include <iomanip>

using namespace std;

struct Node {
    char data;
    unsigned freq;
    Node *left, *right;
    Node(char data, unsigned freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

class MyPriorityQueue {
private:
    vector<Node*> heap;
    
    void heapify(int i) {
        int smallest = i, l = 2 * i + 1, r = 2 * i + 2;
        if (l < heap.size() && heap[l]->freq < heap[smallest]->freq) smallest = l;
        if (r < heap.size() && heap[r]->freq < heap[smallest]->freq) smallest = r;
        if (smallest != i) { 
            swap(heap[i], heap[smallest]); 
            heapify(smallest); 
        }
    }
    
    void bubbleUp(int i) {
        int parent = (i - 1) / 2;
        while (i > 0 && heap[i]->freq < heap[parent]->freq) {
            swap(heap[i], heap[parent]);
            i = parent;
            parent = (i - 1) / 2;
        }
    }
    
public:
    int size() { return heap.size(); }
    
    bool isEmpty() { return heap.empty(); }
    
    void push(Node* node) {
        heap.push_back(node);
        bubbleUp(heap.size() - 1);
    }
    
    Node* extractMin() {
        if (isEmpty()) {
            cout << "Blad: Kolejka jest pusta!\n";
            return nullptr;
        }
        Node* root = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) heapify(0);
        return root;
    }
    
    // Build Queue z tablicy
    void buildQueue(vector<Node*>& nodes) {
        heap = nodes;
        // Budowanie kopca od dolu do gory 
        for (int i = (heap.size() / 2) - 1; i >= 0; i--) {
            heapify(i);
        }
    }
    
    // Decrease Key
    void decreaseKey(int index, unsigned newFreq) {
        if (index < 0 || index >= heap.size()) {
            cout << "Blad: Nieprawidlowy indeks.\n";
            return;
        }
        if (newFreq > heap[index]->freq) {
            cout << "Blad: Nowy priorytet musi byc mniejszy (to jest Min-Heap).\n";
            return;
        }
        heap[index]->freq = newFreq;
        bubbleUp(index);
        cout << "Priorytet zmieniony pomyslnie.\n";
    }
    
    // Wyswietlanie zawartosci kolejki
    void printQueue() {
        if (isEmpty()) {
            cout << "Kolejka jest pusta.\n";
            return;
        }
        cout << "\n=== AKTUALNY STAN KOPCA ===\n";
        cout << "Indeks | Znak | Czestotliwosc\n";
        cout << "--------------------------------\n";
        for (int i = 0; i < heap.size(); i++) {
            cout << setw(6) << i << " | ";
            if (heap[i]->data == '$') 
                cout << "WEZEL";
            else if (heap[i]->data == '\n')
                cout << " \\n  ";
            else if (heap[i]->data == ' ')
                cout << " SPC ";
            else
                cout << "  " << heap[i]->data << "  ";
            cout << " | " << setw(13) << heap[i]->freq << "\n";
        }
        cout << "================================\n\n";
    }
    
    // Metoda pomocnicza do czyszczenia kolejki
    void clear() {
        // Nie usuwamy Node*, bo mogą być używane gdzie indziej
        heap.clear();
    }
};

void storeCodes(Node* root, string str, map<char, string>& codes) {
    if (!root) return;
    if (!root->left && !root->right) codes[root->data] = str;
    storeCodes(root->left, str + "0", codes);
    storeCodes(root->right, str + "1", codes);
}

// --- DEMO KOLEJKI PRIORYTETOWEJ ---
void queueDemo() {
    MyPriorityQueue pq;
    bool running = true;
    
    cout << "\n=== DEMO KOLEJKI PRIORYTETOWEJ ===\n";
    cout << "Kolejka jest teraz pusta.\n";
    
    while (running) {
        cout << "\nOPERACJE:\n";
        cout << "1. Dodaj pojedynczy element (Insert)\n";
        cout << "2. Pobierz element o najmniejszym priorytecie (Extract Min)\n";
        cout << "3. Zbuduj cala kolejke z wlasnych danych (Build)\n";
        cout << "4. Zmniejsz priorytet elementu (Decrease Key)\n";
        cout << "5. Sprawdz czy pusta (IsEmpty)\n";
        cout << "6. Pokaz zawartosc kolejki\n";
        cout << "7. Powrot do menu glownego\n";
        cout << "Wybor: ";
        
        int choice;
        cin >> choice;
        cin.ignore(); // Czyszczenie bufora
        
        switch (choice) {
            case 1: {
                char c;
                unsigned freq;
                cout << "Podaj znak: ";
                cin >> c;
                cout << "Podaj czestotliwosc (liczbe): ";
                cin >> freq;
                cin.ignore();
                
                pq.push(new Node(c, freq));
                cout << "Dodano element [" << c << ": " << freq << "]\n";
                break;
            }
            
            case 2: {
                Node* min = pq.extractMin();
                if (min) {
                    cout << "Pobrano i usunieto element: [";
                    if (min->data == '$') cout << "WEZEL";
                    else cout << "'" << min->data << "'";
                    cout << ", freq: " << min->freq << "]\n";
                }
                break;
            }
            
            case 3: {
                cout << "--- BUDOWANIE KOLEJKI (Nadpisuje obecna!) ---\n";
                int count;
                cout << "Ile elementow chcesz wprowadzic? ";
                cin >> count;
                cin.ignore();
                
                if (count <= 0) {
                    cout << "Liczba elementow musi byc wieksza od 0.\n";
                    break;
                }
                
                vector<Node*> nodes;
                for (int i = 0; i < count; i++) {
                    char c;
                    unsigned freq;
                    cout << "Element " << (i + 1) << "/" << count << ":\n";
                    cout << "  Podaj znak: ";
                    cin >> c;
                    cout << "  Podaj czestotliwosc: ";
                    cin >> freq;
                    cin.ignore();
                    
                    nodes.push_back(new Node(c, freq));
                }
                
                pq.clear();
                pq.buildQueue(nodes);
                cout << "Kolejka zostala zbudowana z " << count << " elementow.\n";
                break;
            }
            
            case 4: {
                cout << "Poglad indeksow:\n";
                pq.printQueue();
                int idx;
                unsigned newFreq;
                cout << "Podaj INDEKS w tablicy (patrz wyzej): ";
                cin >> idx;
                cout << "Podaj nowa, MNIEJSZA czestotliwosc: ";
                cin >> newFreq;
                cin.ignore();
                
                pq.decreaseKey(idx, newFreq);
                break;
            }
            
            case 5: {
                cout << "Czy kolejka jest pusta? " << (pq.isEmpty() ? "TAK" : "NIE") << "\n";
                break;
            }
            
            case 6: {
                pq.printQueue();
                break;
            }
            
            case 7: {
                running = false;
                break;
            }
            
            default:
                cout << "Nie ma takiej opcji.\n";
        }
    }
}

// --- REALNA KOMPRESJA BINARNA ---
void compressFile() {
    string inN, outN;
    cout << "Plik we: "; 
    cin >> inN;
    cout << "Plik wy: "; 
    cin >> outN;

    ifstream f(inN.c_str(), ios::binary);
    if (!f) {
        cout << "Blad: Nie mozna otworzyc pliku wejsciowego!\n";
        return;
    }
    string content((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    f.close();

    if (content.empty()) {
        cout << "Plik jest pusty!\n";
        return;
    }

    map<char, unsigned> freq;
    for (char ch : content) freq[ch]++;

    // Tworzenie wektora wezlow (wydajniejsze niz wielokrotne push)
    vector<Node*> nodes;
    for (auto const& p : freq) 
        nodes.push_back(new Node(p.first, p.second));

    // Budowanie kolejki z gotowej tablicy
    MyPriorityQueue pq;
    pq.buildQueue(nodes);

    while (pq.size() > 1) {
        Node *l = pq.extractMin(), *r = pq.extractMin();
        Node *p = new Node('$', l->freq + r->freq);
        p->left = l; p->right = r;
        pq.push(p);
    }

    map<char, string> codes;
    storeCodes(pq.extractMin(), "", codes);

    ofstream out(outN.c_str(), ios::binary);
    
    // 1. Slownik (tekstowo dla ulatwienia)
    for (auto const& p : codes) {
        if (p.first == '\n') out << "\\n:" << p.second << " ";
        else if (p.first == ' ') out << "SPC:" << p.second << " ";
        else if (p.first == ':') out << "COL:" << p.second << " ";
        else out << p.first << ":" << p.second << " ";
    }
    out << "\n";

    // 2. Przygotowanie bitow
    string bitString = "";
    for (char ch : content) bitString += codes[ch];

    // 3. Zapis dopelnienia (padding) - ile bitow dodano na koncu
    unsigned char padding = (8 - (bitString.length() % 8)) % 8;
    out.put(padding);

    // 4. Pakowanie bitow w bajty i zapis binarny
    unsigned char byte = 0;
    int bitCount = 0;
    for (char b : bitString) {
        byte <<= 1;
        if (b == '1') byte |= 1;
        bitCount++;
        if (bitCount == 8) {
            out.put(byte);
            byte = 0;
            bitCount = 0;
        }
    }
    // Ostatni bajt jesli zostal
    if (bitCount > 0) {
        byte <<= (8 - bitCount);
        out.put(byte);
    }
    
    out.close();
    
    cout << "\n=== KOMPRESJA ZAKONCZONA SUKCESEM ===\n";
    cout << "Rozmiar wejsciowy: " << content.length() << " bajtow\n";
    
    ifstream outFile(outN.c_str(), ios::binary | ios::ate);
    if (outFile) {
        cout << "Rozmiar wyjsciowy: " << outFile.tellg() << " bajtow\n";
        outFile.close();
    }
}

// --- REALNA DEKOMPRESJA BINARNA ---
void decompressFile() {
    string inN, outN;
    cout << "Plik do dek: "; 
    cin >> inN;
    cout << "Plik wynik: "; 
    cin >> outN;

    ifstream in(inN.c_str(), ios::binary);
    if (!in) {
        cout << "Blad: Nie mozna otworzyc pliku do dekompresji!\n";
        return;
    }
    
    string dictLine;
    getline(in, dictLine);

    unsigned char padding;
    in.read((char*)&padding, 1);

    // Odczyt reszty pliku jako bajty i zamiana na bity
    string bitString = "";
    unsigned char byte;
    while (in.read((char*)&byte, 1)) {
        bitString += bitset<8>(byte).to_string();
    }
    in.close();

    // Usuwamy dopelnienie
    if (padding > 0 && bitString.length() >= padding) {
        bitString.erase(bitString.length() - padding);
    }

    // Parsowanie slownika
    map<string, char> rev;
    size_t start = 0, end;
    while ((end = dictLine.find(' ', start)) != string::npos) {
        string entry = dictLine.substr(start, end - start);
        size_t sep = entry.find(':');
        if (sep != string::npos) {
            string key = entry.substr(0, sep), val = entry.substr(sep + 1);
            char r;
            if (key == "\\n") r = '\n'; 
            else if (key == "SPC") r = ' ';
            else if (key == "COL") r = ':'; 
            else r = key[0];
            rev[val] = r;
        }
        start = end + 1;
    }

    ofstream out(outN.c_str(), ios::binary);
    string cur = "";
    for (char b : bitString) {
        cur += b;
        if (rev.count(cur)) {
            out.put(rev[cur]);
            cur = "";
        }
    }
    out.close();
    cout << "\n=== DEKOMPRESJA ZAKONCZONA POMYSLNIE ===\n";
}

int main() {
    int m;
    cout << "\n========================================\n";
    cout << "   PROGRAM DO KODOWANIA HUFFMANA\n";
    cout << "========================================\n";
    
    while (true) {
        cout << "\nWYBIERZ OPCJE:\n";
        cout << "1. Demo Kolejki Priorytetowej\n";
        cout << "2. Kompresuj plik\n";
        cout << "3. Dekompresuj plik\n";
        cout << "0. Koniec\n";
        cout << "Twoj wybor: ";
        
        cin >> m;
        cin.ignore();
        
        if (m == 0) {
            cout << "Do widzenia!\n";
            break;
        }
        
        switch (m) {
            case 1:
                queueDemo();
                break;
            case 2:
                compressFile();
                break;
            case 3:
                decompressFile();
                break;
            default:
                cout << "Nieprawidlowa opcja.\n";
        }
    }
    
    return 0;
}
