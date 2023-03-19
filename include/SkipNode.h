struct SkipNode {
    int val;
    SkipNode *next_level = nullptr;
    SkipNode *next_node = nullptr;

    SkipNode() = default;

    explicit SkipNode(int value)
        : val(value) {}

    explicit SkipNode(SkipNode *level)
        : next_level(level) {}

    void insert_node(SkipNode *node) {
        SkipNode *temp = next_node;
        next_node = node;
        node->next_node = temp;
    }

    /*
    移除同层的下一个结点
    */
    void rm_node() {
        if (next_node == nullptr) return;
        SkipNode *temp = next_node;
        next_node = temp->next_node;
        delete temp;
    }
};