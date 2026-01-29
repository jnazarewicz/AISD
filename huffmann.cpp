#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <bitset>

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
        if (smallest != i) { swap(heap[i], heap[smallest]); heapify(smallest); }
    }
public:
    int size() { return heap.size(); }
    void push(Node* node) {
        heap.push_back(node);
        int i = heap.size() - 1;
        while (i != 0 && heap[(i - 1) / 2]->freq > heap[i]->freq) {
            swap(heap[i], heap[(i - 1) / 2]);
            i = (i - 1) / 2;
        }
    }
    Node* pop() {
        Node* root = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        heapify(0);
        return root;
    }
};

void storeCodes(Node* root, string str, map<char, string>& codes) {
    if (!root) return;
    if (!root->left && !root->right) codes[root->data] = str;
    storeCodes(root->left, str + "0", codes);
    storeCodes(root->right, str + "1", codes);
}

// --- REALNA KOMPRESJA BINARNA ---
void compressFile() {
    string inN, outN;
    cout << "Plik we: "; cin >> inN;
    cout << "Plik wy: "; cin >> outN;

    ifstream f(inN.c_str(), ios::binary);
    if (!f) return;
    string content((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    f.close();

    if (content.empty()) return;

    map<char, unsigned> freq;
    for (char ch : content) freq[ch]++;

    MyPriorityQueue pq;
    for (auto const& p : freq) pq.push(new Node(p.first, p.second));

    while (pq.size() > 1) {
        Node *l = pq.pop(), *r = pq.pop();
        Node *p = new Node('$', l->freq + r->freq);
        p->left = l; p->right = r;
        pq.push(p);
    }

    map<char, string> codes;
    storeCodes(pq.pop(), "", codes);

    ofstream out(outN.c_str(), ios::binary);
    
    // 1. Słownik (tekstowo dla ułatwienia)
    for (auto const& p : codes) {
        if (p.first == '\n') out << "\\n:" << p.second << " ";
        else if (p.first == ' ') out << "SPC:" << p.second << " ";
        else if (p.first == ':') out << "COL:" << p.second << " ";
        else out << p.first << ":" << p.second << " ";
    }
    out << "\n";

    // 2. Przygotowanie bitów
    string bitString = "";
    for (char ch : content) bitString += codes[ch];

    // 3. Zapis dopełnienia (padding) - ile bitów dodano na końcu
    unsigned char padding = (8 - (bitString.length() % 8)) % 8;
    out.put(padding);

    // 4. Pakowanie bitów w bajty i zapis binarny
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
    // Ostatni bajt jeśli został
    if (bitCount > 0) {
        byte <<= (8 - bitCount);
        out.put(byte);
    }
    
    out.close();
    cout << "Skompresowano binarnie! Sprawdz rozmiar pliku.\n";
}

// --- REALNA DEKOMPRESJA BINARNA ---
void decompressFile() {
    string inN, outN;
    cout << "Plik do dek: "; cin >> inN;
    cout << "Plik wynik: "; cin >> outN;

    ifstream in(inN.c_str(), ios::binary);
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

    // Usuwamy dopełnienie
    if (padding > 0 && bitString.length() >= padding) {
        bitString.erase(bitString.length() - padding);
    }

    // Parsowanie słownika (jak wcześniej)
    map<string, char> rev;
    size_t start = 0, end;
    while ((end = dictLine.find(' ', start)) != string::npos) {
        string entry = dictLine.substr(start, end - start);
        size_t sep = entry.find(':');
        if (sep != string::npos) {
            string key = entry.substr(0, sep), val = entry.substr(sep + 1);
            char r;
            if (key == "\\n") r = '\n'; else if (key == "SPC") r = ' ';
            else if (key == "COL") r = ':'; else r = key[0];
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
    cout << "Zdekompresowano pomyslnie.\n";
}

int main() {
    int m;
    cout << "\n1. Kompresja | 2. Dekompresja | 0. Koniec: ";
    while(cin >> m && m != 0) {
        if (m == 1) compressFile();
        if (m == 2) decompressFile();
    }
    return 0;
}