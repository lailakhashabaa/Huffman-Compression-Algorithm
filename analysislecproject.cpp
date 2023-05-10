
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <unordered_map>
#include <ctime>
#include"binary_file_reader.h"
#include"binary_file_writer.h"
using namespace std;
bool encode = 1;


struct node
{
    char letter;
    int frequency;
    node* l;
    node* r;
};
node* CreateNode(char let, int f, node* left, node* right)
{
    node* n = new node;
    n->letter = let;
    n->frequency = f;
    n->l =left ;
    n->r = right;

    return n;
}
//this is a comparator to be able to flip the priority queue from being a maximum heap to a minimum one 
struct comp
{
    bool operator()(node* l, node* r)
    {
        // highest priority item has lowest frequency
        return l->frequency > r->frequency;
    }
};

void encoder(node* root, string s, unordered_map<char, string>& huffcode)
{
    if (root == nullptr)
        return;

    // this will mean that we reached a leaf node
    if (!root->l && !root->r) {
        huffcode[root->letter] = s;

    }

    encoder(root->l, s + "0", huffcode);
    encoder(root->r, s + "1", huffcode);
}

void decode(node* root,BinaryFileReader* br, string& out)
{
    string str = "";
    node* curr = root;
    
    while(br->hasBits()) {
        if (br->getNextBit())
        {
            curr = curr->r;
        }
        else
        {
            curr = curr->l;
        }
        if (curr->l == NULL && curr->r == NULL)
        {
            str += curr->letter;
            curr = root;
        }
    }
    out = str;
}

node* readTreeHelper(string &treeFile)
{
   if(treeFile.size()) {

       char c = treeFile[0];
       treeFile = treeFile.substr(1);
        
        if (c == '1') {
            
            c = treeFile[0];
            treeFile = treeFile.substr(1);
           
            node* lf = CreateNode(c, 0, NULL, NULL);
            return lf;
        }
        else {
            node* intr = CreateNode('$', 0, NULL, NULL);
            intr->l = readTreeHelper(treeFile);
            intr->r = readTreeHelper(treeFile);
            return intr;
        }
    }
    return NULL; // replaceme!
}

node* readTree(string treeFile)
{
    return readTreeHelper(treeFile);
}



void saveTree(node* current,ofstream *treeFile)
{
   
    if (!current->l && !current->r) {
        *(treeFile) << '1';
        *(treeFile)<< (current->letter);
    }
    else {
        *(treeFile) << '0';
        saveTree(current->l, treeFile);
        saveTree(current->r, treeFile);
    }
}

void makeTreeString(node* current, string &str, char &nodesLength)
{

    if (!current->l && !current->r) {
        str+= '1';
        str+= (current->letter);
        nodesLength++;
    }
    else {
        str+=  '0';
        makeTreeString(current->l, str,nodesLength);
        makeTreeString(current->r, str,nodesLength);
    }
}

void NodeTree(string text, bool encode)
{

       
    if (encode == true)
    {
        
        ofstream ofile;
        ofstream outfile;
        if (text == "")
        {
            return;
        }
        // looping over each letter in the input string and storing the frequency
        unordered_map<char, int> frequency;
        for (char letter : text)
        {
            frequency[letter]++;
        }
       
       
        priority_queue<node*, vector<node*>, comp> flippedqueue;
        //creating the nodes for the tree, starting from the least frequency, here we assign the right and left pointers to
        //null since we still did not create internal nodes
        for (auto pair : frequency)
        {
            node* x = CreateNode(pair.first, pair.second, nullptr, nullptr);
            flippedqueue.push(x);
        }
        while (flippedqueue.size() != 1)
        {
            node* l = flippedqueue.top();
            flippedqueue.pop();
            node* r = flippedqueue.top();
            flippedqueue.pop();

            int sumOfFrequencies = l->frequency + r->frequency;
            flippedqueue.push(CreateNode('$', sumOfFrequencies, l, r));
        }

        node* root = flippedqueue.top();


        string treeString = "";
        char nodesLength = 0;

        makeTreeString(root,treeString,nodesLength);
        BinaryFileWriter bfw("output.txt");
        bfw.writeByte(nodesLength);
        int k = 0;
        for (int i = 0; i < nodesLength;)
        {
            if (treeString[k] == '1')
            {
                bfw.writeBit(1);
                k++;
                bfw.writeByte(treeString[k]);
                i++;
            }
            else
            {
                bfw.writeBit(0);
               
            }
            k++;
            
        }
        

        //Will contain character and its binary string according to frequency and tree
        unordered_map<char, string> frequencytree;
        encoder(root, "", frequencytree);

    
       
        // print encoded string
        string s = "";
        for (char letter : text) {
            s += frequencytree[letter];
        }

        for (int i = 0; i < s.size(); i++)
        {
            if (s[i] == '0')
            {
                bfw.writeBit(0);
            }
            else
            {
                bfw.writeBit(1);
            }
        }
        bfw.close();
    }
    else if (encode == false)
    {
       
        ofstream ofile;
        ifstream outfile;
        if (text == "")
        {
            return;
        }
       

        BinaryFileReader bfr("output.txt");


        string treeString = "";
        char nodesLength = 0;
        
        //Now we have to retrieve treeStringBack;

        nodesLength = bfr.getNextByte();
        int k = 0;

        for (int i = 0; i < (int)nodesLength;)
        {
            if (bfr.getNextBit())
            {
                treeString+= "1";
                treeString += bfr.getNextByte();
                i++;
                
            }
            else
            {
                treeString += "0";
               
            }
        }

        node* root = readTree(treeString);  
        
        ofstream outF;

        outF.open("decoded.txt");
        string output;
     
        decode(root, &bfr,output);

        outF << output;
        outF.close();

        cout << endl << endl << "Reminder: Please manually clear the output.txt file to encode a new message" << endl;
    }
}



void processFiles(string inputFile, string outputFile, bool encode)
{
    
        ifstream input;
        input.open("input.txt");
        if (input.fail())
        {
            cout << "file was not opened";
            exit(0);
        }
        string line, text;
        while (getline(input, line))
        {
            text = text + line;
        }

        NodeTree(text, encode);
  
   
}
int main()
{
    cout << "Before starting, please create three text files called \"input\", \"output\" and \"Tree\" in the same directory as the source file." << endl << endl;
    cout << "The \"input\" file will recieve the text you will want to encode. The huffman code will be outputted in the \"output\" file." << endl << endl;
    cout << "When decoding, the program will use the encoded text in the \"output\" file as its input." << endl << endl;
    cout << "Please make sure to clear the \"output\" file after decompressing (decoding)." << endl << endl << endl << endl;
    cout << "Would you like to encode a file or decode a file" << endl << endl << "1. Compress" << endl << endl << "2. Decompress" << endl;
    int i;
    cin >> i;

    while (i != 1 && i != 2)
    {
        cout << "Please enter 1 to Compress or 2 to Decompress" << endl;
        cin >> i;
    }

    if (i == 1)
    {
        encode = true;
    }
    else if (i == 2)
    {
        encode = false;
    }
    processFiles("input.txt", "output.txt", encode);
    return 0;
}
