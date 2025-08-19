#pragma once

#include <vector>
#include <stdexcept>
#include <iterator>
#include <type_traits>

template<typename T>
class ring_buffer {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    // 前向声明迭代器
    template<bool IsConst>
    class ring_iterator;
    
    using iterator = ring_iterator<false>;
    using const_iterator = ring_iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    std::vector<T> buffer_;
    size_type head_ = 0;    // 写入位置
    size_type tail_ = 0;    // 读取位置
    bool full_ = false;     // 是否已满
    size_type capacity_ = 0; // 缓冲区容量

    size_type next_index(size_type index) const noexcept {
        return (index + 1) % capacity_;
    }

    size_type prev_index(size_type index) const noexcept {
        return (index + capacity_ - 1) % capacity_;
    }

public:
    // 构造函数
    ring_buffer() = default;
    
    explicit ring_buffer(size_type capacity) : buffer_(capacity), capacity_(capacity) {}
    
    ring_buffer(size_type capacity, const T& value) : buffer_(capacity, value), capacity_(capacity) {}
    
    template<typename InputIt>
    ring_buffer(size_type capacity, InputIt first, InputIt last) : buffer_(capacity), capacity_(capacity) {
        for (auto it = first; it != last && !full(); ++it) {
            push_back(*it);
        }
    }

    ring_buffer(std::initializer_list<T> init) {
        resize(init.size());
        for (const auto& item : init) {
            push_back(item);
        }
    }

    // 调整缓冲区大小
    void resize(size_type new_capacity) {
        if (new_capacity == 0) {
            clear();
            buffer_.clear();
            capacity_ = 0;
            return;
        }
        
        std::vector<T> new_buffer(new_capacity);
        size_type new_head = 0;
        
        // 复制现有元素到新缓冲区
        for (size_type i = 0; i < size(); ++i) {
            new_buffer[i] = std::move((*this)[i]);
        }
        new_head = size();
        
        buffer_ = std::move(new_buffer);
        head_ = new_head;
        tail_ = 0;
        full_ = (new_head == new_capacity);
        capacity_ = new_capacity;
    }
    
    // 获取容量
    size_type capacity() const noexcept { return capacity_; }
    
    // 获取当前元素数量
    size_type size() const noexcept {
        if (capacity_ == 0) return 0;
        if (full_) return capacity_;
        if (head_ >= tail_) return head_ - tail_;
        return capacity_ - tail_ + head_;
    }
    
    // 检查是否为空
    bool empty() const noexcept {
        return (capacity_ == 0) || (!full_ && (head_ == tail_));
    }
    
    // 检查是否已满
    bool full() const noexcept {
        return (capacity_ > 0) && full_;
    }

    // 元素访问
    reference front() {
        if (empty()) throw std::runtime_error("ring_buffer is empty");
        return buffer_[tail_];
    }
    
    const_reference front() const {
        if (empty()) throw std::runtime_error("ring_buffer is empty");
        return buffer_[tail_];
    }
    
    reference back() {
        if (empty()) throw std::runtime_error("ring_buffer is empty");
        return buffer_[prev_index(head_)];
    }
    
    const_reference back() const {
        if (empty()) throw std::runtime_error("ring_buffer is empty");
        return buffer_[prev_index(head_)];
    }

    reference at(size_type pos) {
        if (pos >= size()) throw std::out_of_range("ring_buffer index out of range");
        return buffer_[(tail_ + pos) % capacity_];
    }
    
    const_reference at(size_type pos) const {
        if (pos >= size()) throw std::out_of_range("ring_buffer index out of range");
        return buffer_[(tail_ + pos) % capacity_];
    }
    
    reference operator[](size_type pos) {
        return buffer_[(tail_ + pos) % capacity_];
    }
    
    const_reference operator[](size_type pos) const {
        return buffer_[(tail_ + pos) % capacity_];
    }

    // 修改操作
    void push_back(const T& value) {
        if (capacity_ == 0) return;
        
        buffer_[head_] = value;
        head_ = next_index(head_);
        
        if (full_) {
            tail_ = next_index(tail_);
        } else if (head_ == tail_) {
            full_ = true;
        }
    }
    
    void push_back(T&& value) {
        if (capacity_ == 0) return;
        
        buffer_[head_] = std::move(value);
        head_ = next_index(head_);
        
        if (full_) {
            tail_ = next_index(tail_);
        } else if (head_ == tail_) {
            full_ = true;
        }
    }

    template<typename... Args>
    reference emplace_back(Args&&... args) {
        if (capacity_ == 0) throw std::runtime_error("ring_buffer has zero capacity");
        
        buffer_[head_] = T(std::forward<Args>(args)...);
        reference result = buffer_[head_];
        head_ = next_index(head_);
        
        if (full_) {
            tail_ = next_index(tail_);
        } else if (head_ == tail_) {
            full_ = true;
        }
        
        return result;
    }

    void pop_front() {
        if (empty()) throw std::runtime_error("ring_buffer is empty");
        tail_ = next_index(tail_);
        full_ = false;
    }

    void clear() noexcept {
        head_ = tail_ = 0;
        full_ = false;
    }

    // 迭代器
    iterator begin() noexcept { return iterator(*this, 0); }
    const_iterator begin() const noexcept { return const_iterator(*this, 0); }
    const_iterator cbegin() const noexcept { return const_iterator(*this, 0); }
    
    iterator end() noexcept { return iterator(*this, size()); }
    const_iterator end() const noexcept { return const_iterator(*this, size()); }
    const_iterator cend() const noexcept { return const_iterator(*this, size()); }
    
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

    // 比较操作
    bool operator==(const ring_buffer& other) const {
        if (size() != other.size()) return false;
        for (size_type i = 0; i < size(); ++i) {
            if ((*this)[i] != other[i]) return false;
        }
        return true;
    }
    
    bool operator!=(const ring_buffer& other) const {
        return !(*this == other);
    }

    // 迭代器实现
    template<bool IsConst>
    class ring_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<IsConst, const T*, T*>;
        using reference = std::conditional_t<IsConst, const T&, T&>;
        using buffer_type = std::conditional_t<IsConst, const ring_buffer&, ring_buffer&>;

    private:
        buffer_type buffer_;
        size_type pos_;

    public:
        ring_iterator(buffer_type buffer, size_type pos) : buffer_(buffer), pos_(pos) {}
        
        template<bool OtherConst>
        ring_iterator(const ring_iterator<OtherConst>& other, 
                     std::enable_if_t<IsConst || !OtherConst, int> = 0)
            : buffer_(other.buffer_), pos_(other.pos_) {}

        reference operator*() const { return buffer_[pos_]; }
        pointer operator->() const { return &buffer_[pos_]; }
        
        ring_iterator& operator++() { ++pos_; return *this; }
        ring_iterator operator++(int) { auto tmp = *this; ++pos_; return tmp; }
        
        ring_iterator& operator--() { --pos_; return *this; }
        ring_iterator operator--(int) { auto tmp = *this; --pos_; return tmp; }
        
        ring_iterator& operator+=(difference_type n) { pos_ += n; return *this; }
        ring_iterator& operator-=(difference_type n) { pos_ -= n; return *this; }
        
        ring_iterator operator+(difference_type n) const { return ring_iterator(buffer_, pos_ + n); }
        ring_iterator operator-(difference_type n) const { return ring_iterator(buffer_, pos_ - n); }
        
        difference_type operator-(const ring_iterator& other) const { return pos_ - other.pos_; }
        
        reference operator[](difference_type n) const { return buffer_[pos_ + n]; }
        
        bool operator==(const ring_iterator& other) const { return pos_ == other.pos_; }
        bool operator!=(const ring_iterator& other) const { return pos_ != other.pos_; }
        bool operator<(const ring_iterator& other) const { return pos_ < other.pos_; }
        bool operator<=(const ring_iterator& other) const { return pos_ <= other.pos_; }
        bool operator>(const ring_iterator& other) const { return pos_ > other.pos_; }
        bool operator>=(const ring_iterator& other) const { return pos_ >= other.pos_; }

        friend ring_iterator operator+(difference_type n, const ring_iterator& it) {
            return it + n;
        }
    };
};

// 便利函数
template<typename T>
typename ring_buffer<T>::iterator operator+(
    typename ring_buffer<T>::difference_type n,
    const typename ring_buffer<T>::iterator& it) {
    return it + n;
}
