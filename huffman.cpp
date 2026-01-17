#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>

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

// --- KOMPRESJA ---
void compressFile() {
    string inN, outN;
    cout << "Plik wejsciowy: "; cin >> inN;
    cout << "Plik wynikowy: "; cin >> outN;

    ifstream f(inN.c_str(), ios::binary);
    if (!f) { cout << "Blad pliku!\n"; return; }
    string content((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    f.close();

    if (content.empty()) return;

    map<char, unsigned> freq;
    for (size_t i = 0; i < content.length(); i++) freq[content[i]]++;

    MyPriorityQueue pq;
    for (auto const& pair : freq) pq.push(new Node(pair.first, pair.second));

    while (pq.size() > 1) {
        Node *l = pq.pop(), *r = pq.pop();
        Node *p = new Node('$', l->freq + r->freq);
        p->left = l; p->right = r;
        pq.push(p);
    }

    map<char, string> codes;
    storeCodes(pq.pop(), "", codes);

    ofstream outFile(outN.c_str(), ios::binary);
    // Zapis słownika z mapowaniem
    for (auto const& pair : codes) {
        char ch = pair.first;
        if (ch == '\n') outFile << "\\n:" << pair.second << " ";
        else if (ch == '\r') outFile << "\\r:" << pair.second << " ";
        else if (ch == ' ')  outFile << "SPC:" << pair.second << " ";
        else if (ch == ':')  outFile << "COL:" << pair.second << " ";
        else outFile << ch << ":" << pair.second << " ";
    }
    outFile << "\n"; // TYLKO JEDEN ENTER po słowniku
    
    for (size_t i = 0; i < content.length(); i++) outFile << codes[content[i]];
    outFile.close();
    cout << "Skompresowano pomyslnie.\n";
}

// --- DEKOMPRESJA ---
void decompressFile() {
    string inN, outN;
    cout << "Plik do dekompresji: "; cin >> inN;
    cout << "Plik wynikowy: "; cin >> outN;

    ifstream in(inN.c_str(), ios::binary);
    if (!in) return;

    string dictLine, encoded;
    getline(in, dictLine); // Wczytaj słownik
    
    // Kluczowe: wczytujemy resztę pliku jako bity, ignorując białe znaki na końcu
    char ch;
    while (in.get(ch)) {
        if (ch == '0' || ch == '1') encoded += ch;
    }
    in.close();

    map<string, char> rev;
    size_t start = 0, end;
    while ((end = dictLine.find(' ', start)) != string::npos) {
        string entry = dictLine.substr(start, end - start);
        size_t sep = entry.find(':');
        if (sep != string::npos) {
            string key = entry.substr(0, sep);
            string val = entry.substr(sep + 1);
            char real;
            if (key == "\\n") real = '\n';
            else if (key == "\\r") real = '\r';
            else if (key == "SPC") real = ' ';
            else if (key == "COL") real = ':';
            else real = key[0];
            rev[val] = real;
        }
        start = end + 1;
    }

    ofstream out(outN.c_str(), ios::binary);
    string cur = "";
    for (size_t i = 0; i < encoded.length(); i++) {
        cur += encoded[i];
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
    while(true) {
        cout << "\n1. Kompresja | 2. Dekompresja | 0. Koniec: ";
        cin >> m;
        if (m == 1) compressFile();
        else if (m == 2) decompressFile();
        else if (m == 0) break;
    }
    return 0;
}