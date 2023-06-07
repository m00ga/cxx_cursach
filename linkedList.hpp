#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <functional>
#include <iterator>
#include <memory>
#include <string>
#include <utility>

namespace ezlib {

template <class T> class LinkedList {
  public:
    class Node {
        friend class LinkedList;

      public:
        std::shared_ptr<T> data;
        Node *prev = nullptr;
        Node *next = nullptr;

        explicit Node(T value) { data = std::make_shared<T>(value); }
        Node() {}
    };

    std::shared_ptr<Node *> pbeg;
    std::shared_ptr<Node *> pend;

    int size;

    LinkedList<T>() noexcept {
        pbeg = std::make_shared<Node *>();
        pend = std::make_shared<Node *>(new Node);
        *pbeg = *pend;
        size = 0;
    }

    ~LinkedList<T>() noexcept { clear(); }

    class Iterator;

    Iterator begin() { return Iterator(*pbeg); }

    Iterator end() { return Iterator(*pend); }

    void push_back(const T &data);
    template <typename K = T> void remove(const K &key);
    template <typename Compare, typename K>
    void remove(const K &key, const Compare &comp) {
        for (auto it = begin(); it != end(); ++it) {
            if (comp(*it, key)) {
                _delNode(it.currentNode);
                --size;
            }
        }
    }
    template <typename Compare = std::less<T>> void sort(Compare comp);
    void sort();
    template <typename K = T>
    std::pair<Iterator, Iterator> find_range(const K &key);

    template <typename K = T> Iterator find(const K &key);
    template <typename K, typename Compare>
    Iterator find_if_linear(const K &key, const Compare &comp);
    template <typename K = T, typename Compare>
    Iterator find_if(const K &key, Compare comp);
    template <typename K = T, typename Compare>
    std::pair<Iterator, Iterator> find_range_if(const K &key, Compare comp);
    void clear();

    class Iterator {
        friend class LinkedList;

      private:
        Node *currentNode;

      public:
        Iterator() : currentNode(nullptr) {}
        explicit Iterator(Node *node) : currentNode(node) {}
        Iterator(const Iterator &it) : currentNode(it.currentNode) {}

        Iterator &operator=(Node *node) {
            this->currentNode = node;
            return *this;
        }

        Iterator &operator++() {
            if (currentNode)
                currentNode = currentNode->next;
            return *this;
        }

        Iterator &operator--() {
            if (currentNode)
                currentNode = currentNode->prev;
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        int operator-(const Iterator &it) {
            int counter = 0;
            if (currentNode->prev != nullptr) {
                while (it.currentNode != currentNode) {
                    currentNode = currentNode->prev;
                    counter++;
                }
            }

            return counter;
        }

        Iterator operator-(int n) {
            Iterator tmp(*this);
            for (; n > 0; n--) {
                if (tmp.currentNode->prev != nullptr) {
                    tmp.currentNode = tmp.currentNode->prev;
                } else {
                    break;
                }
            }
            return tmp;
        }

        Iterator operator+(int n) {
            Iterator tmp(*this);
            for (; n > 0; n--) {
                if (tmp.currentNode->next != nullptr) {
                    tmp.currentNode = tmp.currentNode->next;
                } else {
                    break;
                }
            }
            return tmp;
        }

        Iterator &operator+=(int n) {
            for (; n > 0; n--) {
                if (currentNode->next != nullptr) {
                    currentNode = currentNode->next;
                } else {
                    break;
                }
            }
            return *this;
        }

        Iterator &operator-=(int n) {
            for (; n > 0; n--) {
                if (currentNode->prev != nullptr) {
                    currentNode = currentNode->prev;
                } else {
                    break;
                }
            }
            return *this;
        }

        Iterator &swap(Iterator &it) {
            T temp = *(currentNode->data);
            *(currentNode->data) = *(it.currentNode->data);
            *(it.currentNode->data) = temp;
            return *this;
        }

        T &operator*() { return *(currentNode->data); }

        T *operator->() { return &*(currentNode->data); }

        bool operator!=(const Iterator &it) {
            return currentNode != it.currentNode;
        }

        bool operator==(const Iterator &it) {
            return currentNode == it.currentNode;
        }
    };

  private:
    void _delNode(Node *node) {
        if (node == *pbeg) {
            *pbeg = node->next;
            (*pbeg)->prev = nullptr;
        } else {
            (node->prev)->next = node->next;
            (node->next)->prev = node->prev;
        }
    }
};
template <typename T> using Iterator = typename LinkedList<T>::Iterator;

template <typename T, typename K = T>
Iterator<T> lower_bound(Iterator<T> begin, Iterator<T> end, const K &key) {
    Iterator<T> it;
    --end;
    int count = end - begin;
    int step;
    while (count > 0) {
        it = begin;
        step = count / 2;
        it += step;

        if (*it < key) {
            begin = ++it;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return begin;
}

template <typename T, typename K = T, typename Compare>
Iterator<T> lower_bound(Iterator<T> begin, Iterator<T> end, const K &key,
                        Compare comp) {
    Iterator<T> it;
    --end;
    int count = end - begin;
    int step;
    while (count > 0) {
        it = begin;
        step = count / 2;
        it += step;

        if (comp(*it, key)) {
            begin = ++it;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return begin;
}

template <typename T, typename K = T>
Iterator<T> upper_bound(Iterator<T> begin, Iterator<T> end, const K &key) {
    Iterator<T> it;
    --end;
    int count = end - begin;
    int step;
    while (count > 0) {
        it = begin;
        step = count / 2;
        it += step;

        if (!(key < *it)) {
            begin = ++it;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return begin;
}

template <typename T, typename K = T, typename Compare>
Iterator<T> upper_bound(Iterator<T> begin, Iterator<T> end, const K &key,
                        Compare comp) {
    Iterator<T> it;
    --end;
    int count = end - begin;
    int step;
    while (count > 0) {
        it = begin;
        step = count / 2;
        it += step;

        if (!comp(key, *it)) {
            begin = ++it;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return begin;
}

template <typename T> void LinkedList<T>::clear() {
    Iterator it = begin();
    Iterator it_s;
    while (it != end() && size != 0) {
        it_s = it;
        ++it;
        _delNode(it_s.currentNode);
        // it_s.currentNode->del();
    }
    (*pend)->prev = nullptr;
    *pbeg = *pend;
    size = 0;
}

template <typename T> void LinkedList<T>::push_back(const T &data) {
    Node *n = new Node(data);
    if (*pbeg == *pend) {
        *pbeg = n;
        (*pbeg)->next = *pend;
        (*pend)->prev = n;
    } else {
        n->next = *pend;
        n->prev = (*pend)->prev;
        (*pend)->prev->next = n;
        (*pend)->prev = n;
    }
    size++;
}

template <typename T>
template <typename K>
void LinkedList<T>::remove(const K &key) {
    // Iterator it = lower_bound<T, K>(begin(), end(), key);
    // if (it != end()) {
    //     it.currentNode->del();
    //     size--;
    // } else {
    //     throw std::runtime_error("Nothing found");
    // }
    for (auto it = begin(); it != end(); ++it) {
        if (*it == key) {
            _delNode(it.currentNode);
            // it.currentNode->del();
            --size;
        }
    }
}

template <typename T>
template <typename Compare>
void LinkedList<T>::sort(Compare comp) {
    for (Iterator i = begin(); i != end(); ++i) {
        for (Iterator j = begin(); j != i; ++j) {
            if (comp(*i, *j)) {
                i.swap(j);
            }
        }
    }
}

template <typename T> void LinkedList<T>::sort() {
    sort<std::less<T>>(std::less<T>());
}

template <typename T>
template <typename K>
std::pair<Iterator<T>, Iterator<T>> LinkedList<T>::find_range(const K &key) {
    auto lower = lower_bound<T, K>(begin(), end(), key);
    if (lower == --end()) {
        throw std::runtime_error("Not found");
    }
    auto upper = upper_bound<T, K>(begin(), end(), key);
    return {lower, upper};
}

template <typename T>
template <typename K>
Iterator<T> LinkedList<T>::find(const K &key) {
    auto lower = lower_bound<T, K>(begin(), end(), key);
    if (lower == --end()) {
        throw std::runtime_error("Not found");
    }
    return lower;
}

template <typename T>
template <typename K, typename Compare>
Iterator<T> LinkedList<T>::find_if(const K &key, Compare comp) {
    auto lower = lower_bound<T, K, Compare>(begin(), end(), key, comp);
    if (lower == --end()) {
        throw std::runtime_error("Not found");
    }
    return lower;
}

template <typename T>
template <typename K, typename Compare>
Iterator<T> LinkedList<T>::find_if_linear(const K &key, const Compare &comp) {
    if (size == 0) {
        throw std::runtime_error("List is empty!");
    }
    for (Iterator it = begin(); it != end(); ++it) {
        if (comp(*it, key)) {
            return it;
        }
    }
    throw std::runtime_error("Not found");
}

template <typename T>
template <typename K, typename Compare>
std::pair<Iterator<T>, Iterator<T>> LinkedList<T>::find_range_if(const K &key,
                                                                 Compare comp) {
    auto lower = lower_bound<T, K, Compare>(begin(), end(), key, comp);
    if (lower == --end()) {
        throw std::runtime_error("Not found");
    }
    auto upper = upper_bound<T, K, Compare>(begin(), end(), key, comp);
    return {lower, upper};
}

}; // namespace ezlib

#endif
