#include "HafmanTree.h"

std::vector<bool> operator| (std::vector<bool>& v1, std::vector<bool>& v2) {
    std::vector<bool> ans(256, 0);
    for (size_t i = 0; i < ans.size(); i++)
    {
        ans[i] = (bool)(v1[i] | v2[i]);
    }
    return ans;
}

std::vector<bool>& operator+= (std::vector<bool>& v1, std::vector<bool>& v2) {
    v1.insert(v1.end(), v2.begin(), v2.end());
    return v1;
}

// Node methods
HafmanTree::Node::Node(const std::vector<bool>& chrs)
    : _left(nullptr)
    , _right(nullptr)
    , _count(1)
    , _chr(chrs) {}


HafmanTree::Node::Node(const unsigned char chr, const int count)
    : _left(nullptr)
    , _right(nullptr)
    , _count(count)
    , _chr(256)
{
    _chr[chr] = 1;
}

HafmanTree::Node::Node(const std::vector<bool>& chrs, Node* left, Node* right, const int count)
    : _left(left)
    , _right(right)
    , _count(count)
    , _chr(chrs)
{}

HafmanTree::Node::Node(Node* first, Node* second)
    : _left(first)
    , _right(second)
    , _count(first->_count + second->_count)
    , _chr(first->_chr | second-> _chr)
{}

const HafmanTree::Node* HafmanTree::Node::left() const {
    return _left;
}
const HafmanTree::Node* HafmanTree::Node::right() const {
    return _right;
}
HafmanTree::Node* HafmanTree::Node::left()
{
    return _left;
}
HafmanTree::Node* HafmanTree::Node::right()
{
    return _right;
}
int HafmanTree::Node::count() const {
    return _count;
}
void HafmanTree::Node::left(Node* left) {
    _left = left;
}
void HafmanTree::Node::right(Node* right) {
    _right = right;
}
void HafmanTree::Node::count(const int count) {
    _count = count;
}

unsigned char HafmanTree::Node::first_chr() const
{
    for (unsigned char i = 0; i < _chr.size(); i++)
    {
        if (_chr[i]) {
            return i;
        }
    }
    return 255;
}

bool HafmanTree::Node::has_chr(unsigned char chr) const
{
    return (bool)_chr[chr];
}

HafmanTree::HafmanTree()
    : _root(nullptr)
    , _textSize(0)
{}

HafmanTree::HafmanTree(const std::string& input_text_file_name, const std::string& import_tree_file_name, bool has_tree)
{
    if (has_tree) {
        std::list<Node*> nodes;

        import_list(import_tree_file_name, nodes);
        build(nodes);
    }
    else {
        build(input_text_file_name);
    }
}

HafmanTree::~HafmanTree()
{
    _clear_subtree(_root);
    _root = nullptr;
}

// Hafman tree methods
void HafmanTree::build(std::list<Node*> nodes)
{
    nodes.sort([](Node* first, Node* second)
               {
                   return first->count() < second->count();
               });
    while (nodes.size() != 1) {
        Node* first = nodes.front();
        nodes.pop_front();
        Node* second = nodes.front();
        nodes.pop_front();
        Node* subtree_root = new Node(first, second);

        std::list<Node*>::iterator seeker = nodes.begin();
        while (seeker != nodes.end() && (*seeker)->count() <= subtree_root->count()) {
            seeker++;
        }
        nodes.insert(seeker, subtree_root);
    }
    _root = nodes.front();
}

void HafmanTree::build(const std::string& input_text_file)
{
    std::list<Node*> nodes;
    build_list(input_text_file, nodes);

    build(nodes);
    export_list(nodes);
}


double HafmanTree::encode(const std::string& input_text_file_name, const std::string encoded_file_name)
{
    if (!_root)
    {
        build(input_text_file_name);
    }

    double after_compression = 0.0;
    std::ifstream input_text_file(input_text_file_name + ".txt");
    std::ofstream encoded_stream(encoded_file_name + ".bin", std::ios::out | std::ios::binary);

    // if (!input_text_file && !encoded_stream) {
    //     throw std::exception("error");
    // }

    unsigned char ch;
    std::vector<int> binary_textt(0);
    std::vector<bool> binary_text(0);
    std::vector<bool> code_for_key(0);
    while (input_text_file >> std::noskipws >> ch)
    {
        _find_code_for_key(code_for_key, ch);
        binary_text += code_for_key;
        after_compression += code_for_key.size();
        code_for_key.clear();

        if (binary_text.size() % 8 == 0)
        {
            char* el = new char[binary_text.size() / 8];
            for (size_t i = 0; i < binary_text.size() / 8; i++)
            {
                unsigned char temp = 0;
                for (size_t j = 0; j < 8; j++)
                {
                    temp |= (binary_text[i * 8 + j] << 7);
                    if (j < 7)
                        temp >>= 1;
                }
                el[i] = temp;
            }
            encoded_stream.write(el, binary_text.size() / 8);
            delete[] el;
            binary_text.clear();
        }
    }
    int count_minor_bits = 0;
    if (binary_text.size())
    {

        char* el = new char[binary_text.size() / 8 + (binary_text.size() % 8 > 0)];
        for (int i = 0; i < binary_text.size() / 8 + (binary_text.size()%8>0); i++)
        {
            unsigned char temp = 0;
            for (int j = 0; j < std::min(8, ((int)binary_text.size()) - i * 8); j++)
            {
                temp |= (binary_text[i * 8 + j] << std::min(8, ((int)binary_text.size()) - i * 8 + 1) - 1);
                if (j < 7)
                    temp >>= 1;
            }
            el[i] = temp;
        }
        encoded_stream.write(el, binary_text.size() / 8 + (binary_text.size()%8 > 0));
        delete[] el;
        count_minor_bits = 8 - binary_text.size() % 8;
    }

    encoded_stream.write((const char*)&count_minor_bits, sizeof(char));

    double before_compression = _textSize * 8.0;
    return (after_compression / before_compression) * 100;
}

bool HafmanTree::decode(const std::string& encoded_file_name, const std::string& decoded_file_name) const
{
    std::ifstream encoded_text(encoded_file_name + ".bin", std::ios::binary);
    std::ofstream decoded_text(decoded_file_name + ".txt");

    iteration_search(encoded_text, decoded_text);

    encoded_text.close();
    decoded_text.close();

    return true;
}

void HafmanTree::export_list(const std::list<Node*>& nodes) const
{
    std::ofstream exp_tree("tree.txt");
    if (!exp_tree) {
        std::cout << "Cannot open file\n";
        return;
    }
    for (Node* node : nodes) {
        exp_tree << (int)node->first_chr() << " ";
        exp_tree << node->count() << " ";
    }
    exp_tree.close();
}

void HafmanTree::import_list(const std::string& file_name, std::list<HafmanTree::Node*>& nodes)
{
    std::ifstream imp_tree(file_name + ".txt");
    assert(imp_tree);

    unsigned char ch;
    int count;

    while (imp_tree)
    {
        imp_tree >> ch;
        assert(imp_tree);
        imp_tree >> count;
        _textSize += count;

        nodes.push_back(new Node(ch, count));
    }
    imp_tree.close();
}

void HafmanTree::iteration_search(std::ifstream& encoded_stream, std::ofstream& decoded_text) const
{
    Node* runner = _root;
    unsigned char first;
    unsigned char second;
    unsigned char third;
    encoded_stream.read((char*)&first, sizeof(char));
    encoded_stream.read((char*)&second, sizeof(char));
    if (!encoded_stream)
    {
        return;
    }

    while (encoded_stream.read((char*)&third, sizeof(char)))
    {
        unsigned char mask = 1;
        while (true)
        {
            for (; (runner->left() || runner->right()) && (mask != 0); ) {
                runner = first & mask ? runner->right() : runner->left();
                mask <<= 1;
            }
            if (!mask) {
                break;
            }
            else {
                decoded_text << runner->first_chr();
                runner = _root;
            }
        }
        first = second;
        second = third;
    }
    unsigned char mask = 1;
    int i = 0;
    while (true)
    {
        for (; (runner->left() || runner->right()) && (mask != 0) && i < second; i++) {
            runner = first & mask ? runner->right() : runner->left();
            mask <<= 1;
        }
        if (!mask) {
            break;
        }
        else {
            decoded_text << runner->first_chr();
            runner = _root;
            if (i == second) {
                break;
            }
        }
    }
}

void HafmanTree::_find_code_for_key(Node* root, std::vector<bool>& code, unsigned char ch) const
{
    if (!root->left() && !root->right())
    {
        return;
    }

    if (root->left()->has_chr(ch)) {
        code.push_back(0);
        _find_code_for_key(root->left(), code, ch);
    }
    else {
        code.push_back(1);
        _find_code_for_key(root->right(), code, ch);
    }
}

void HafmanTree::_find_code_for_key(std::vector<bool>& code, unsigned char ch) const
{
    _find_code_for_key(_root, code, ch);
}

void HafmanTree::_clear_subtree(Node* root)
{
    if (!root)
    {
        return;
    }

    _clear_subtree(root->left());
    _clear_subtree(root->right());

    delete root;
}

void HafmanTree::build_list(const std::string& file_name, std::list<HafmanTree::Node*>& nodes)
{
    std::vector<bool> chars(256);
    int counts[256];

    std::ifstream text(file_name + ".txt");
    if (!text) {
        std::cout << "No such file";
    }
    unsigned char ch;

    while (text >> std::noskipws >> ch) {
        _textSize++;
        if (chars[ch] == 0) {
            counts[ch] = 1;
            chars[ch] = 1;
        }
        else {
            counts[ch]++;
        }
    }
    text.close();

    for (int i = 0; i < chars.size(); i++)
    {
        if (chars[i] == true) {
            nodes.push_back(new Node(i, counts[i]));
        }
    }
}

