#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <bitset>
#include <fstream>

using namespace std;

// Node structure for Huffman Tree
struct Node {
    char ch;
    unsigned freq;
    Node *left, *right;

    Node(char character, unsigned frequency) {
        left = right = nullptr;
        ch = character;
        freq = frequency;
    }
};

// Comparison class for priority queue
struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};



Node* buildHuffmanTree(const string& text) {
    unordered_map<char, unsigned> frequency;
    for (char ch : text) {
        frequency[ch]++;
    }

    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto pair : frequency) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        Node *left = pq.top();
        pq.pop();
        Node *right = pq.top();
        pq.pop();

        Node *merged = new Node('$', left->freq + right->freq);
        merged->left = left;
        merged->right = right;

        pq.push(merged);
    }

    return pq.top();
}



void generateHuffmanCodes(Node* root, const string& str, unordered_map<char, string>& huffmanCodes) {
    if (root == nullptr) return;

    if (root->ch != '$') {
        huffmanCodes[root->ch] = str;
    }

    generateHuffmanCodes(root->left, str + "0", huffmanCodes);
    generateHuffmanCodes(root->right, str + "1", huffmanCodes);
}


string encodeText(const string& text, const unordered_map<char, string>& huffmanCodes) {
    string encodedText;
    for (char ch : text) {
        encodedText += huffmanCodes.at(ch);
    }
    return encodedText;
}


string decodeText(Node* root, const string& encodedText) {
    string decodedText;
    Node* current = root;

    for (char bit : encodedText) {
        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }

        if (current->left == nullptr && current->right == nullptr) {
            decodedText += current->ch;
            current = root;
        }
    }

    return decodedText;
}


void compressFile(const string& inputFile, const string& outputFile, unordered_map<char, string>& huffmanCodes, Node*& root) {
    ifstream inFile(inputFile);
    string text((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    root = buildHuffmanTree(text);
    generateHuffmanCodes(root, "", huffmanCodes);
    string encodedText = encodeText(text, huffmanCodes);

    ofstream outFile(outputFile, ios::binary);
    bitset<8> bits;
    for (size_t i = 0; i < encodedText.size(); ++i) {
        bits[i % 8] = encodedText[i] == '1';
        if (i % 8 == 7 || i == encodedText.size() - 1) {
            outFile.put(static_cast<unsigned char>(bits.to_ulong()));
        }
    }
    outFile.close();
}

void decompressFile(const string& encodedFile, const string& outputFile, Node* root) {
    ifstream inFile(encodedFile, ios::binary);
    string encodedText;
    char byte;
    while (inFile.get(byte)) {
        bitset<8> bits(byte);
        for (size_t i = 0; i < 8; ++i) {
            encodedText += bits[i] ? '1' : '0';
        }
    }
    inFile.close();

    string decodedText = decodeText(root, encodedText);

    ofstream outFile(outputFile);
    outFile << decodedText;
    outFile.close();
}


int main() {
    unordered_map<char, string> huffmanCodes;
    Node* root = nullptr;

    compressFile("input.txt", "compressed.bin", huffmanCodes, root);
    decompressFile("compressed.bin", "decompressed.txt", root);

    return 0;
}
