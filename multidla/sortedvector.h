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

    explicit sorted_vector(const Allocator& a = Allocator()): mVec(a) { }
    explicit sorted_vector(size_type n, const T& value= T(), const Allocator& a = Allocator()):
        mVec(n, value, a) { }
    template <class InputIterator>
    sorted_vector(InputIterator first, InputIterator last, const Allocator& a = Allocator()):
			mVec(first, last, a) { std::sort(mVec.begin(), mVec.end()); }
    sorted_vector(const std::vector<T,Allocator>& x): mVec(x) {
        std::sort(mVec.begin(), mVec.end());
    }

    virtual ~sorted_vector() { }

    template <class InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
        mVec.assign(first, last);
    }
    
    void assign(size_type n, const T& u)
    {
        mVec.assign(n, u);
    }

    void add(const T& val)
    {
        if (mVec.empty()) {
            mVec.push_back(val);
        } else {
            int place = this->find_insert_pos(val);
            // if (this->binary_search(0, mVec.size(), val, place))
            if (place != -1) {
                if (place == mVec.size()) {
                    mVec.push_back(val);
                } else {
                    typename std::vector<T>::iterator it = mVec.begin() + place;
                    mVec.insert(it, val);
                }
            } else {
                // don't insert repeated value
            }
        }
    }

    const_reference at(size_type n) const
    {
        return mVec.at(n);
    }

    reference at(size_type n)
    {
        return mVec.at(n);
    }

    reference back()
    {
        return mVec.back();
    }

    const_reference back() const
    {
        return mVec.back();
    }

    iterator begin()
    {
        return mVec.begin();
    }
    
    const_iterator begin() const
    {
        return mVec.begin();
    }

    size_type capacity() const
    {
        return mVec.capacity();
    }

    void clear()
    {
        mVec.clear();
    }

    bool empty() const
    {
        return mVec.empty();
    }

    iterator end()
    {
        return mVec.end();
    }
    
    const_iterator end() const
    {
        return mVec.end();
    }

    reference front()
    {
        return mVec.front();
    }
    
    const_reference front() const
    {
        return mVec.front();
    }

    reference operator[](size_type n)
    {
        return mVec[n];
    }
    
    const_reference operator[](size_type n) const
    {
        return mVec[n];
    }
    
    size_type size() const
    {
        return mVec.size();
    }

    reverse_iterator rbegin()
    {
        return mVec.rbegin();
    }
    
    const_reverse_iterator rbegin() const
    {
        return mVec.rbegin();
    }
    
    reverse_iterator rend()
    {
        return mVec.rend();
    }
    
    const_reverse_iterator rend() const
    {
        return mVec.rend();
    }

    void del(const T& val)
    {
        for (iterator it = this->begin(); it < this->end(); ++it) {
            if (*it == val) {
                this->mVec.erase(it);
                return;
            }
        }
    }

private:
    std::vector<T> mVec;

    int find_insert_pos(const T& key) const
    {
        int res = 0;
        for (const_iterator it = mVec.begin(); it < mVec.end(); ++it) {
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
