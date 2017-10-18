#ifndef MULTIDLA_SORTEDVECTOR_H
#define	MULTIDLA_SORTEDVECTOR_H

#include <algorithm>
#include <vector>

template <class T, class Allocator = std::allocator<T> >
class sorted_vector : public std::vector<T, Allocator>
{
public:
    typedef typename std::vector<T, Allocator>::iterator iterator;
    typedef typename std::vector<T, Allocator>::const_iterator const_iterator;
    typedef typename std::vector<T, Allocator>::reverse_iterator reverse_iterator;
    typedef typename std::vector<T, Allocator>::const_reverse_iterator const_reverse_iterator;
    typedef typename std::vector<T, Allocator>::const_reference const_reference;
    typedef typename std::vector<T, Allocator>::reference reference;
    typedef typename std::vector<T, Allocator>::size_type size_type;

    // it will be a wrapper to Vector STL class
    // but elements will be sorted.
    // So we re implement insert operator - it will choose place automatically
    // Maybe class must be thread-safe?
    // Search will be binary - so we must use vector class for fastest search

    explicit sorted_vector(const Allocator& a = Allocator()): m_vec(a) { }
    explicit sorted_vector(size_type n, const T& value= T(), const Allocator& a = Allocator()):
        m_vec(n, value, a) { }
    template <class InputIterator>
    sorted_vector(InputIterator first, InputIterator last, const Allocator& a = Allocator()):
            m_vec(first, last, a) { std::sort(m_vec.begin(), m_vec.end()); }
    sorted_vector(const std::vector<T,Allocator>& x): m_vec(x) {
        std::sort(m_vec.begin(), m_vec.end());
    }

    virtual ~sorted_vector() { }

    template <class InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
        m_vec.assign(first, last);
    }
    
    void assign(size_type n, const T& u)
    {
        m_vec.assign(n, u);
    }

    void add(const T& val)
    {
        if (m_vec.empty()) {
            m_vec.push_back(val);
        } else {
            int32_t place = this->find_insert_pos(val);
            // if (this->binary_search(0, mVec.size(), val, place))
            if (place != -1) {
                if (place == m_vec.size()) {
                    m_vec.push_back(val);
                } else {
                    typename std::vector<T>::iterator it = m_vec.begin() + place;
                    m_vec.insert(it, val);
                }
            } else {
                // don't insert repeated value
            }
        }
    }

    const_reference at(size_type n) const
    {
        return m_vec.at(n);
    }

    reference at(size_type n)
    {
        return m_vec.at(n);
    }

    reference back()
    {
        return m_vec.back();
    }

    const_reference back() const
    {
        return m_vec.back();
    }

    iterator begin()
    {
        return m_vec.begin();
    }
    
    const_iterator begin() const
    {
        return m_vec.begin();
    }

    size_type capacity() const
    {
        return m_vec.capacity();
    }

    void clear()
    {
        m_vec.clear();
    }

    bool empty() const
    {
        return m_vec.empty();
    }

    iterator end()
    {
        return m_vec.end();
    }
    
    const_iterator end() const
    {
        return m_vec.end();
    }

    reference front()
    {
        return m_vec.front();
    }
    
    const_reference front() const
    {
        return m_vec.front();
    }

    reference operator[](size_type n)
    {
        return m_vec[n];
    }
    
    const_reference operator[](size_type n) const
    {
        return m_vec[n];
    }
    
    size_type size() const
    {
        return m_vec.size();
    }

    reverse_iterator rbegin()
    {
        return m_vec.rbegin();
    }
    
    const_reverse_iterator rbegin() const
    {
        return m_vec.rbegin();
    }
    
    reverse_iterator rend()
    {
        return m_vec.rend();
    }
    
    const_reverse_iterator rend() const
    {
        return m_vec.rend();
    }

    void del(const T& val)
    {
        for (iterator it = this->begin(); it < this->end(); ++it) {
            if (*it == val) {
                this->m_vec.erase(it);
                return;
            }
        }
    }

private:
    std::vector<T> m_vec;

    int32_t find_insert_pos(const T& key) const
    {
        int32_t res = 0;
        for (const_iterator it = m_vec.begin(); it < m_vec.end(); ++it) {
            if (*it > key) {
                return res;
            } else if (*it == key) {
                return -1;
            }
            ++res;
        }
        return res;
    }
};

#endif	// MULTIDLA_SORTEDVECTOR_H
