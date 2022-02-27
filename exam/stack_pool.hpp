#include <vector>
#include <iterator>


//POOL CLASS DECLARATION
template<typename T, typename N>
class stack_pool;


//ITERATOR CLASS
//they allow us to iterate through single stacks
template<typename T, typename N>
class _iterator {
    // node index
    N node;
    // Pool
    stack_pool<T, N>* pool;

public:
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    // Constructor for _iterator
    _iterator(stack_pool<T, N>* p, N n) : pool{ p }, node{ n }{}

    // Pointer
    T& operator*() noexcept { return pool->value(node); }

    // Pre-increment
    _iterator& operator++() noexcept {
        node = pool->next(node);
        return *this;
    }

    // Post-increment
    _iterator& operator++(int) noexcept {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    // Comparison operators == and !=
    friend bool operator==(const _iterator& x, const _iterator& y) noexcept { return x.node == y.node; }

    // Compare iterators: not equal
    friend bool operator!=(const _iterator& x, const _iterator& y) noexcept { return !(x == y); }
};

//POOL CLASS CONSTRUCTION
template <typename T, typename N = std::size_t>
class stack_pool {

    struct node_t {

        //value of the node
        T value;
        //Index of the node this node points to
        N next;

        // Constructors of the node
        node_t(T val, N n) : value{ val }, next{ n } { }
        node_t() : value{}, next{}  {}
    };

    std::vector<node_t> pool;

    using stack_type = N;
    using value_type = T;
    using size_type = typename std::vector<node_t>::size_type;

    //Stack of free nodes: at the beggining it is empty
    stack_type free_nodes;



    //PRIVATE METHODS
    //Given a stack, return references to the head
    node_t& node(stack_type x) noexcept { return pool[x - 1]; }

    const node_t& node(stack_type x) const noexcept { return pool[x - 1]; }

    //Auxiliary function _push
    //returns the new head for the stack
    template<typename X>
    stack_type _push(X&& val, stack_type head) {
        stack_type new_head;

        // if there are no free nodes, add a new node to the pool
        if (empty(free_nodes)) {
            pool.emplace_back(node_t(val, head));
            new_head = stack_type(pool.size());
        }

        // otherwise use the head of the stack free_nodes
        else {
            new_head = free_nodes;
            free_nodes = next(free_nodes);
            value(new_head) = std::forward<X>(val);
            next(new_head) = head;
        }
        return new_head;
    }

public:
    //Constructor for pools
    stack_pool() { free_nodes = end(); }
    //Constructor for pool, aslo reserves n nodes in the pool
    explicit stack_pool(size_type n) {
        free_nodes = end();
        reserve(n);
    }

    //Iterators for pools
    //derived fro class _iterator
    using iterator = _iterator<T, N>;
    using const_iterator = _iterator<const T, N>;

    iterator begin(stack_type x) noexcept { return iterator{ this,x }; }
    iterator end(stack_type) noexcept { return iterator{ this, end() }; }

    const_iterator begin(stack_type x) const noexcept { return const_iterator{ this,x }; }
    const_iterator end(stack_type) const noexcept { return const_iterator{ this, end() }; }

    const_iterator cbegin(stack_type x) const noexcept { return const_iterator{ this,x }; }
    const_iterator cend(stack_type) const noexcept { return const_iterator{ this, end() }; }

    //Pool end
    stack_type end() const noexcept { return stack_type(0); }

    //Creates a new stack
    stack_type new_stack() { return end(); }


    // Reserves n nodes in the pool
    void reserve(size_type n) { pool.reserve(n); }

    // Returns the capacity of the pool
    size_type capacity() const { return pool.capacity(); }

    // Checks if a stack is empty
    bool empty(stack_type x) const noexcept { return (x == end()); }
    
    // Given a stack Returns the value of the head
    T& value(stack_type x) noexcept { return node(x).value; }

    const T& value(stack_type x) const noexcept { return node(x).value; }

    // Returns the index of the next value in a stack x
    stack_type& next(stack_type x) noexcept { return node(x).next; }

    const stack_type& next(stack_type x) const noexcept { return node(x).next;  }

    // Push a node in a stack, and give it a value
    stack_type push(const T& val, stack_type head) noexcept { return _push(val, head); }

    stack_type push(T&& val, stack_type head) noexcept { return _push(std::move(val), head); }

    
    
    //Move the head of the stack s1 to the stack s2
    //Return the new head of s1
    stack_type move_head(stack_type s1, stack_type s2) {
        stack_type new_head = next(s1);
        next(s1) = s2;
        s2 = s1;
        return new_head;
    }
    stack_type pop(stack_type x) noexcept {
        stack_type new_head = next(x);
        next(x) = free_nodes;
        free_nodes = x;
        return new_head;
    }

    stack_type free_stack(stack_type x) noexcept {
        // Free entire stack
        // Returns the new index of the stack
        while (!empty(x)) {
            x = pop(x);
        }
        return x;
    }

};
