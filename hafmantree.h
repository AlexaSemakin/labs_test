#pragma once
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <cstddef>
#include <span>
#include <assert.h>

class HafmanTree
{
public:
    class Node;
    HafmanTree();
    HafmanTree(const std::string& input_file_text, const std::string& import_tree_file_name, bool has_tree);
    ~HafmanTree();
    void build(const std::string& input_file_text);
    double encode(const std::string& input_file_text, const std::string decoded_file_name);
    bool decode(const std::string& encoded_file_name, const std::string& decoded_file_name) const;

private:
    void import_list(const std::string& file_name, std::list<Node*>& nodes);
    void export_list(const std::list<Node*>& nodes) const;
    void iteration_search(std::ifstream& encoded_stream, std::ofstream& decoded_text) const;
    void _find_code_for_key(Node* root, std::vector<bool>& code, unsigned char ch) const;
    void _find_code_for_key(std::vector<bool>& code, unsigned char ch) const;
    void _clear_subtree(Node* root);
    void build(std::list<Node*> nodes);
    void build_list(const std::string& file_name, std::list<Node*>& nodes);
    Node* _root = nullptr;
    int _textSize = 0;
};

class HafmanTree::Node {
public:
    Node(const std::vector<bool>& chrs);
    Node(const unsigned char chr, const int count);
    Node(const std::vector<bool>& chrs, Node* left, Node* right, const int count);
    Node(Node* first, Node* second);
    ~Node() = default;
    const Node* left() const;
    const Node* right() const;
    Node* left();
    Node* right();
    int count() const;
    void left(Node* left);
    void right(Node* right);
    void count(const int count);
    unsigned char first_chr() const;
    bool has_chr(unsigned char ch) const;

private:
    Node* _left = nullptr;
    Node* _right = nullptr;
    int _count = 1;
    std::vector<bool> _chr;
};

