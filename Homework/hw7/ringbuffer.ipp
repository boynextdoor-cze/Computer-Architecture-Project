// Constructor
template <typename _Tp>
RingBuffer<_Tp>::RingBuffer()
{
    // Initialization
    _M_capacity = RING_BUFFER_INIT_SIZE;
    _M_read_pos = 0;
    _M_write_pos = 0;
    _M_content = new _Tp[RING_BUFFER_INIT_SIZE];
}

// Deconstructor
template <typename _Tp>
RingBuffer<_Tp>::~RingBuffer()
{
    delete[] _M_content;
}

// Retrieve capacity
template <typename _Tp>
typename RingBuffer<_Tp>::size_type
RingBuffer<_Tp>::get_capacity() const
{
    return _M_capacity;
}

// Check if the ringbuffer is empty
template <typename _Tp>
bool RingBuffer<_Tp>::is_empty() const
{
    // Check if content is legal
    if (_M_content == NULL)
        return false;
    return _M_read_pos == _M_write_pos;
}

// Grow the size of buffer
template <typename _Tp>
void RingBuffer<_Tp>::grow()
{
    size_t new_capacity;
    // Scenario 1: size doesn't exceed 1024
    if (_M_capacity < 1024)
        new_capacity = _M_capacity * RING_BUFFER_GROW_FACTOR1;
    // Scenario 2: size exceeds 1024
    else
        new_capacity = _M_capacity * RING_BUFFER_GROW_FACTOR2;
    // Count the number of entries
    size_t s = _M_read_pos;
    size_t t = _M_write_pos;
    size_t cnt = (t - s + _M_capacity) % _M_capacity;
    // Create a temporary array to hold the arranged data
    _Tp *new_content = new _Tp[new_capacity];
    // Move elements from old array to new array
    int i = 0;
    while (!is_empty())
        read(new_content[i++]);
    // The origin vector should be freed after being useless
    delete[] _M_content;
    // Update buffer variables
    _M_content = new_content;
    _M_capacity = new_capacity;
    _M_read_pos = 0;
    _M_write_pos = cnt;
}

// Read a single data from buffer
template <typename _Tp>
bool RingBuffer<_Tp>::read(_Tp &data)
{
    // Exclude potential invalid operations
    if (is_empty())
        return false;
    // Load data from the buffer
    data = _M_content[_M_read_pos++];
    // Dequeue the data and return true
    _M_read_pos %= _M_capacity;
    return true;
}

// Write single data to buffer
template <typename _Tp>
bool RingBuffer<_Tp>::write(const _Tp &data)
{
    // Calculate the number of entries
    size_t cnt = (_M_write_pos - _M_read_pos + _M_capacity) % _M_capacity;
    // Expand the capacity if buffer is full
    if (cnt == _M_capacity - 1)
        grow();
    // Write data to the buffer
    _M_content[_M_write_pos++] = data;
    _M_write_pos %= _M_capacity;
    // Calculate the number of entries
    cnt = (_M_write_pos - _M_read_pos + _M_capacity) % _M_capacity;
    // Expand the capacity if buffer is full
    if (cnt == _M_capacity - 1)
        grow();
    return true;
}

// Read multi-data from buffer
template <typename _Tp>
bool RingBuffer<_Tp>::read_multi(size_t rdsize, std::vector<_Tp> &data)
{
    // Exclude potential invalid operations
    if (rdsize == 0 || is_empty())
        return false;
    // Calculate the number of entries
    size_t cnt = (_M_write_pos - _M_read_pos + _M_capacity) % _M_capacity;
    // Check if elements are enough to be read
    if (cnt < rdsize)
        return false;
    // Resize vector data if the size doesn't match rdsize
    if (data.size() != rdsize)
        data.resize(rdsize,0);
    int i = 0;
    // Dequeue elements from buffer and put them into data
    while (rdsize--)
    {
        data[i++] = _M_content[_M_read_pos++];
        _M_read_pos %= _M_capacity;
    }
    return true;
}

// Write multi-data to buffer
template <typename _Tp>
bool RingBuffer<_Tp>::write_multi(size_t wrtsize, const std::vector<_Tp> &data)
{
    // Exclude potential invalid operations
    if (wrtsize == 0 || wrtsize > data.size())
        return false;
    // One-by-one write-in process
    for (size_t i = 0; i < wrtsize; ++i)
    {
        // Calculate the number of entries
        size_t cnt = (_M_write_pos - _M_read_pos + _M_capacity) % _M_capacity;
        // Expand the capacity if buffer is full
        if (cnt == _M_capacity - 1)
            grow();
        // Write data to the buffer
        _M_content[_M_write_pos++] = data[i];
        _M_write_pos %= _M_capacity;
        // Calculate the number of entries
        cnt = (_M_write_pos - _M_read_pos + _M_capacity) % _M_capacity;
        // Expand the capacity if buffer is full
        if (cnt == _M_capacity - 1)
            grow();
    }
    return true;
}

// Map function implementation
template <typename _Tp>
bool RingBuffer<_Tp>::map(std::function<_Tp(_Tp)> &&func)
{
    // Fetch two endings
    size_t s = _M_read_pos;
    size_t t = _M_write_pos;
    while (s != t)
    {
        // Traverse the buffer array and apply "func" to each element
        _M_content[s] = func(_M_content[s]);
        s++;
        s %= _M_capacity;
    }
    return true;
}

// Implementation of __iterator
template <typename _Tp>
class __detail::__iterator
{
public:
    // Constructor
    __iterator()
    {
        p = NULL;
        buf_ptr = NULL;
    }

    // Copy constructor
    __iterator(const __iterator &other)
    {
        p = other.p;
        buf_ptr = other.buf_ptr;
    }

    // Initialize the iterator with a pointer
    __iterator(_Tp *ptr, RingBuffer<_Tp> *buffer)
    {
        p = ptr;
        buf_ptr = buffer;
    }

    // Deconstructor
    ~__iterator()
    {
        p = NULL;
        buf_ptr = NULL;
    }

    // Copy assignment operator
    __iterator<_Tp> &operator=(__iterator<_Tp> &other)
    {
        p = other.p;
        return *this;
    }

    // Dereference operator
    _Tp &operator*() const { return *p; }

    // Arrow operator
    _Tp *operator->() const { return p; }

    // Equal
    bool operator==(const __iterator<_Tp> a) const { return p == a.p; }

    // Not equal
    bool operator!=(const __iterator<_Tp> a) const { return p != a.p; }

    // Self increment prefix operator
    __iterator<_Tp> &operator++()
    {
        // Check if the pointer is already at the end of buffer
        if (p == &buf_ptr->_M_content[buf_ptr->_M_capacity - 1])
            p = &buf_ptr->_M_content[0];
        else
            ++p;
        // Return the position of the moved pointer
        return *this;
    }

    // Self decrement prefix operator
    __iterator<_Tp> &operator--()
    {
        // Check if the pointer is already at the start of buffer
        if (p == &buf_ptr->_M_content[0])
            p = &buf_ptr->_M_content[buf_ptr->_M_capacity - 1];
        else
            --p;
        // Return the position of the moved pointer
        return *this;
    }

    // Self increment suffix operator
    __iterator<_Tp> operator++(int)
    {
        __iterator<_Tp> myself = *this;
        // Check if the pointer is already at the end of buffer
        if (p == &buf_ptr->_M_content[buf_ptr->_M_capacity - 1])
            p = &buf_ptr->_M_content[0];
        else
            p++;
        // Return the original pointer
        return myself;
    }

    // Self decrement suffix operator
    __iterator<_Tp> operator--(int)
    {
        __iterator<_Tp> myself = *this;
        // Check if the pointer is already at the start of buffer
        if (p == &buf_ptr->_M_content[0])
            p = &buf_ptr->_M_content[buf_ptr->_M_capacity - 1];
        else
            p--;
        // Return the original pointer
        return myself;
    }

    // Arithmic add
    __iterator<_Tp> operator+(ptrdiff_t x)
    {
        __iterator<_Tp> myself = *this;
        // Execute ++ operation for x times
        while (x--)
            myself++;
        return myself;
    }

    // Arithmic sub
    __iterator<_Tp> operator-(ptrdiff_t x)
    {
        __iterator<_Tp> myself = *this;
        // Execute -- operation for x times
        while (x--)
            myself--;
        return myself;
    }

    // Compound increment
    __iterator<_Tp> &operator+=(ptrdiff_t x)
    {
        // Execute ++ operation for x times
        while (x--)
            p++;
        return *this;
    }

    // Compound decrement
    __iterator<_Tp> &operator-=(ptrdiff_t x)
    {
        // Execute -- operation for x times
        while (x--)
            p--;
        return *this;
    }

    // The pointer to data
    _Tp *p;
    RingBuffer<_Tp> *buf_ptr;
};

// Implementation of __const_iterator
template <typename _Tp>
class __detail::__const_iterator
{
public:
    // Constructor
    __const_iterator()
    {
        p = NULL;
        buf_ptr = NULL;
    }

    // Convert constructor
    __const_iterator(const __iterator<_Tp> &other)
    {
        p = other.p;
        buf_ptr = other.buf_ptr;
    }

    // Copy constructor
    __const_iterator(const __const_iterator<_Tp> &other)
    {
        p = other.p;
        buf_ptr = other.buf_ptr;
    }

    // Initialize the iterator with a pointer
    __const_iterator(const _Tp *ptr, const RingBuffer<_Tp> *buffer)
    {
        p = ptr;
        buf_ptr = buffer;
    }

    // Deconstructor
    ~__const_iterator()
    {
        p = NULL;
        buf_ptr = NULL;
    }

    // Copy assignment operator
    // Copy bewteen the same type
    __const_iterator<_Tp> &operator=(const __const_iterator<_Tp> &other)
    {
        p = other.p;
        buf_ptr = other.buf_ptr;
        return *this;
    }
    // Copy between different types
    __const_iterator<_Tp> &operator=(const __iterator<_Tp> &other)
    {
        p = other.p;
        buf_ptr = other.buf_ptr;
        return *this;
    }

    // Dereference operator
    const _Tp &operator*() const { return *p; }

    // Arrow operator
    const _Tp *operator->() const { return p; }

    // Equal
    bool operator==(const __const_iterator a) const { return p == a.p; }

    // Not equal
    bool operator!=(const __const_iterator a) const { return p != a.p; }

    // Self increment prefix operator
    __const_iterator<_Tp> &operator++()
    {
        // Check if the pointer is already at the end of buffer
        if (p == &buf_ptr->_M_content[buf_ptr->_M_capacity - 1])
            p = &buf_ptr->_M_content[0];
        else
            ++p;
        // Return the position of the moved pointer
        return *this;
    }

    // Self decrement prefix operator
    __const_iterator<_Tp> &operator--()
    {
        // Check if the pointer is already at the start of buffer
        if (p == &buf_ptr->_M_content[0])
            p = &buf_ptr->_M_content[buf_ptr->_M_capacity - 1];
        else
            --p;
        // Return the position of the moved pointer
        return *this;
    }

    // Self increment suffix operator
    __const_iterator<_Tp> operator++(int)
    {
        __const_iterator<_Tp> myself = *this;
        // Check if the pointer is already at the end of buffer
        if (p == &buf_ptr->_M_content[buf_ptr->_M_capacity - 1])
            p = &buf_ptr->_M_content[0];
        else
            p++;
        // Return the original pointer
        return myself;
    }

    // Self decrement suffix operator
    __const_iterator<_Tp> operator--(int)
    {
        __const_iterator<_Tp> myself = *this;
        // Check if the pointer is already at the start of buffer
        if (p == &buf_ptr->_M_content[0])
            p = &buf_ptr->_M_content[buf_ptr->_M_capacity - 1];
        else
            p--;
        // Return the original pointer
        return myself;
    }

    // Arithmic add
    __const_iterator<_Tp> operator+(ptrdiff_t x)
    {
        __const_iterator<_Tp> myself = *this;
        // Execute ++ operation for x times
        while (x--)
            myself++;
        return myself;
    }

    // Arithmic sub
    __const_iterator<_Tp> operator-(ptrdiff_t x)
    {
        __const_iterator<_Tp> myself = *this;
        // Execute -- operation for x times
        while (x--)
            myself--;
        return myself;
    }

    // Compound increment
    __const_iterator<_Tp> &operator+=(ptrdiff_t x)
    {
        // Execute ++ operation for x times
        while (x--)
            p++;
        return *this;
    }

    // Compound decrement
    __const_iterator<_Tp> &operator-=(ptrdiff_t x)
    {
        // Execute -- operation for x times
        while (x--)
            p--;
        return *this;
    }

    // The constant pointer to data
    const _Tp *p;
    const RingBuffer<_Tp> *buf_ptr;
};

// iterator_begin
template <typename _Tp>
typename RingBuffer<_Tp>::iterator
RingBuffer<_Tp>::begin()
{
    return iterator(&_M_content[_M_read_pos], this);
}

// iterator_end
template <typename _Tp>
typename RingBuffer<_Tp>::iterator
RingBuffer<_Tp>::end()
{
    return iterator(&_M_content[_M_write_pos], this);
}

// const_iterator_begin
template <typename _Tp>
typename RingBuffer<_Tp>::const_iterator
RingBuffer<_Tp>::cbegin() const
{
    return const_iterator(&_M_content[_M_read_pos], this);
}

// const_iterator_end
template <typename _Tp>
typename RingBuffer<_Tp>::const_iterator
RingBuffer<_Tp>::cend() const
{
    return const_iterator(&_M_content[_M_write_pos], this);
}
