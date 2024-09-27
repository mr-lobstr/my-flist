#ifndef MY_FORWARD_LIST_GUARD_H
#define MY_FORWARD_LIST_GUARD_H

#include <utility>

struct FListHead {
    FListHead* next = nullptr;
};

template <typename T>
class FList {
    using Head = FListHead;

public:
    FList() noexcept = default;

    FList (FList&& rhs) noexcept {
        if (rhs.empty())
            return;

        prevFirst = std::exchange (rhs.prevFirst, Head{});
        ptrLast = std::exchange (rhs.ptrLast, &rhs.prevFirst);
    }

    FList (FList const& rhs) {
        rhs.for_each_head (
            [&] (auto* pHead) {
                push_back (get_value (pHead));
            }
        );
    }

    FList& operator= (FList&& rhs) noexcept {
        if (this != &rhs) {
            auto tmp = std::move (rhs);
            swap (tmp);
        }
        
        return *this;
    }

    FList& operator= (FList const& rhs) {
        if (this != &rhs) {
            auto tmp = rhs;
            swap (tmp);
        }
        
        return *this;
    }

    ~FList() noexcept {
        for_each_head (
            [] (auto* pHead) {
                delete (get_ptr_node(pHead));
            }
        );
    }

    bool empty() const noexcept {
        return ptrLast == &prevFirst;
    }

    void swap (FList& rhs) noexcept {
        std::swap(*this, rhs);
    }

    void push_back (T const& value) {
        ptrLast->next = new Node {nullptr, value};
        ptrLast = ptrLast->next;
    }

    void push_front (T const& value) {
        auto oldFirst = prevFirst.next;
        prevFirst.next = new Node {oldFirst, value};
    }

    void pop_front() noexcept {
        auto oldFirst = get_ptr_node (begin());
        prevFirst.next = oldFirst->next;
        delete (oldFirst);
    }

    Head* begin() noexcept {
        return prevFirst.next;
    }

    Head const* begin() const noexcept {
        return prevFirst.next;
    }

    Head* end() noexcept {
        return nullptr;
    }

    Head const* end() const noexcept {
        return nullptr;
    }

    T& back() noexcept {
        return get_value (ptrLast);
    }

    T const& back() const noexcept {
        return get_value (ptrLast);
    }

    T& front() noexcept {
        return get_value (begin());
    }

    T const& front() const noexcept {
        return get_value (begin());
    }

    static
    T& get_value (Head* pHead) noexcept {
        return get_ptr_node (pHead)->value;
    }

    static
    T const& get_value (Head const* pHead) noexcept {
        return get_ptr_node (pHead)->value;
    }

private:
    template <typename Action>
    void for_each_head (Action action) const {
        auto beg = prevFirst.next;

        while (beg != nullptr) {
            auto pNext = beg->next;
            action(beg);
            beg = pNext;
        }
    }

    static
    auto* get_ptr_node (Head* pHead) noexcept {
        return static_cast<Node*> (pHead);
    }

    static
    auto* get_ptr_node (Head const* pHead) noexcept {
        return static_cast<Node const*> (pHead);
    }

private:
    struct Node: public Head {
        T value;
    };

    Head prevFirst {};
    Head* ptrLast = const_cast<Head*> (&prevFirst);
};

#endif