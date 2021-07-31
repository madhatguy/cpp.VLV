/**
 * @file VLVector.hpp
 * @author  Guy Lev <thehatguy>
 * @version 1.0
 * @date 7 August 2020
 *
 * @brief A virtual length vector
 *
 * @section DESCRIPTION A STL like container which holds a static memory and can allocate memory dynamically if needed.
 */
#ifndef CPP_EXAM_VLVECTOR_HPP
#define CPP_EXAM_VLVECTOR_HPP

#include <stdexcept>

#define DEFAULT_STATIC_CAPACITY 16

#define STARTING_SIZE 0

#define FIRST_IDX 0

#define INCREASE_INC 1

#define NEXT_ELEM 1

#define INCREASE_FACTOR 3 / 2

#define OUT_OF_RANGE_MSG "VLVector::_M_range_check: __n >= this->size()"

template<class T, size_t StaticCapacity = DEFAULT_STATIC_CAPACITY>

/**
 * A Virtual length vector. Has a static capacity of StaticCapacity, when exceeded the elements are moved to dynamically
 * allocate space.
 * @tparam T The type of the elements.
 */
class VLVector
{
private:


    T _statData[StaticCapacity];
    bool _dynamic = false;
    size_t _size;
    size_t _capacity;
    T *_curData;

    /**
     * @brief Updates the capacity, allocates dynamic memory for a new array, copies all of the elements to it, deletes
     * the previous array if it was dynamically allocated and changes to pointer to point on the new array.
     */
    void _increaseCapacity()
    {
        _capacity = (_size + INCREASE_INC) * INCREASE_FACTOR;
        T *temp = new T[(size_t) (sizeof(T) * _capacity)](); //creates an array of the size specified by the formula.
        for (size_t i = FIRST_IDX; i < _size; ++i)
        {
            temp[i] = data()[i];
        }
        if (_dynamic)
        {
            delete[] _curData;
        }
        _curData = temp, _dynamic = true;
    }

    /**
     * Copies all of the elements to the static array, deletes the previous array, updates the stats accordingly
     */
    void _decreaseCapacity() //being called to only if dynamic == true.
    {
        for (size_t idx = FIRST_IDX; idx < _size; ++idx)
        {
            _statData[idx] = _curData[idx];
        }
        delete[] _curData;
        _curData = &(_statData[FIRST_IDX]);
        _capacity = StaticCapacity;
        _dynamic = false;
    }

public:

    template<bool Const = false>
    /**
     * @brief A template class for VLV iterator/const_iterator.
     * @tparam Const Indicator for const_iterator.
     */
    class Iterator
    {
    public:
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef std::random_access_iterator_tag iterator_category;
        typedef typename std::conditional_t<Const, T const &, T &> reference;
        typedef typename std::conditional_t<Const, T const *, T *> pointer;
    private:
        pointer _cur;
    public:
        /**
         * @brief C'tor for iterator
         * @param cur A pointer to a place in a VLV. casted to const if Const == true.
         */
        Iterator(pointer cur) : _cur{reinterpret_cast<pointer>(cur)} //casting not const to const if needed.
        {
        }

        /**
         * @brief A copy c'tor. For copying iterator to const_iterator, and iterator to itself.
         * @param toCopy The iterator to copy.
         */
        Iterator(const Iterator<false> &toCopy) : _cur(&(*toCopy)) //const iterator has a default copy c'tor for itself
        {
        }

        /**
         * @brief Dereference for a const_iterator.
         * @tparam IsConst The type of the iterator.
         * @return The value of the item referenced by the iterator.
         */
        template<bool IsConst = Const> //so that will appear only in const iterator class.
        std::enable_if_t<IsConst, reference>
        operator*() const noexcept
        {
            return *_cur;
        }


        /**
         * @brief Dereference for iterator.
         * @tparam IsConst The type of the iterator.
         * @return A reference to the value of the item referenced by the iterator.
         */
        template<bool IsConst = Const> //so that will appear only in non-const iterator class.
        std::enable_if_t<!IsConst, reference>
        operator*() const noexcept
        {
            return *_cur;
        }

        /**
         * @brief Assignment operator. works the same as the c'tor.
         * @param ptr A pointer to a place in a VLV. casted to const if Const == true.
         * @return A ref to the current iterator.
         */
        Iterator &operator=(pointer ptr) const
        {
            _cur = ptr;
            return *this;
        }

        /**
         * @brief Same as the copy c'tor. Can copy iterator of the same type or iterator to const_iterator.
         * @param toCopy The iterator to copy.
         * @return A ref to the current iterator.
         */
        Iterator &operator=(Iterator const &toCopy)
        {
            (*this)(toCopy);
            return *this;
        }

        /**
         * Equality check
         */
        friend bool
        operator==(Iterator const &lhs, Iterator const &rhs) //friend - for working on all iterator type combinations
        {
            return lhs._cur == rhs._cur;
        }

        /**
         * Inequality check
         */
        friend bool operator!=(Iterator const &lhs, Iterator const &rhs)
        {
            return lhs._cur != rhs._cur;
        }

        /**
         * Bigger check
         */
        friend bool operator>(Iterator const &lhs, Iterator const &rhs)
        {
            return lhs._cur > rhs._cur;
        }

        /**
         * Bigger/equal check
         */
        friend bool operator>=(Iterator const &lhs, Iterator const &rhs)
        {
            return lhs._cur >= rhs._cur;
        }

        /**
         * Smaller check
         */
        friend bool operator<(Iterator const &lhs, Iterator const &rhs)
        {
            return lhs._cur < rhs._cur;
        }

        /**
         * Small/equal check
         */
        friend bool operator<=(Iterator const &lhs, Iterator const &rhs)
        {
            return lhs._cur <= rhs._cur;
        }

        /**
         * @brief Iterator subtraction.
         * @param lhs An iterator.
         * @param rhs Another iterator.
         * @return The distance between the iterators' pointers.
         */
        friend difference_type operator-(Iterator const &lhs, Iterator const &rhs) noexcept
        {
            difference_type temp(lhs._cur - rhs._cur);
            return temp;
        }

        /**
         * @brief Add rhs to the iterator.
         * @return The iterator after being moved rhs items forward.
         */
        Iterator &operator+=(difference_type const &rhs) noexcept
        {
            _cur += rhs;
            return *this;
        }

        /**
         * @brief Add rhs and the iterator. Doesn't change the iterator's pointer value.
         * @return The iterator after being moved rhs items forward.
         */
        Iterator operator+(difference_type const &rhs) noexcept
        {
            Iterator temp(_cur + rhs);
            return temp;
        }

        /**
         * @brief Subtracts rhs from the iterator.
         * @return The iterator after being moved rhs items backwards.
         */
        Iterator &operator-=(difference_type const &rhs) noexcept
        {
            _cur -= rhs;
            return *this;
        }

        /**
         * @brief Subtracts rhs from the iterator. Doesn't change the iterator's pointer value.
         * @return The iterator after being moved rhs items backwards.
         */
        Iterator operator-(difference_type const &rhs) noexcept
        {
            Iterator temp(_cur - rhs);
            return temp;
        }

        /**
         * @brief A prefix single place addition.
         * @return The iterator after being moved one step forward.
         */
        Iterator &operator++() noexcept
        {
            ++_cur;
            return *this;
        }

        /**
         * @brief A postfix single place addition.
         * @return The iterator before being moved one step forward.
         */
        Iterator operator++(int) noexcept
        {
            Iterator temp = *this;
            ++_cur;
            return temp;
        }

        /**
         * @brief A prefix single place subtraction.
         * @return The iterator after being moved one step backwards.
         */
        Iterator &operator--() noexcept
        {
            --_cur;
            return *this;
        }

        /**
         * @brief A postfix single place subtraction.
         * @return The iterator before being moved one step backwards.
         */
        Iterator operator--(int) noexcept
        {
            Iterator temp = *this;
            --_cur;
            return temp;
        }
    };

    typedef Iterator<false> iterator;
    typedef Iterator<true> const_iterator;

    /**
     * @brief A regular c'tor.
     */
    VLVector() : _size(STARTING_SIZE), _capacity(StaticCapacity)
    {
        _curData = &(_statData[FIRST_IDX]);
    }

    /**
     * @brief A copy ctor.
     * @param toCopy The VLVector to copy.
     */
    VLVector(VLVector const &toCopy)
            : VLVector()
    {
        this->operator=(toCopy);
    }

    /**
     * @brief A c'tor from a set of items.
     * @tparam InputIterator The iterator that is given by the user.
     * @param first The first item to copy.
     * @param last The first item after the items to copy.
     */
    template<class InputIterator>
    VLVector(InputIterator first, InputIterator last) : VLVector()
    {
        while (first != last)
        {
            push_back(*(first++));
        }
    }

    /**
     * @brief Destructor. Frees data if dynamically allocated.
     */
    ~VLVector()
    {
        if (_dynamic)
        {
            delete[] _curData;
        }
    }

    /**
     * @return The current amount of elements in the container.
     */
    size_t size() const
    {
        return _size;
    }

    /**
     * @return The current capacity the container can hold before it will have to grow.
     */
    size_t capacity() const
    {
        return _capacity;
    }

    /**
     * @return true if the container is empty, false otherwise.
     */
    bool empty() const noexcept
    {
        return _size == STARTING_SIZE;
    }

    /**
     * @brief returns the element at place idx. @throws std::out_of_range if the idx is illegal. const version.
     */
    const T &at(const size_t &idx) const
    {
        if (idx >= _size)
        {
            throw std::out_of_range(OUT_OF_RANGE_MSG);
        }
        return operator[](idx);
    }

    /**
     * @brief returns the element at place idx. @throws std::out_of_range if the idx is illegal.
     */
    T &at(const size_t &idx)
    {
        if (idx >= _size)
        {
            throw std::out_of_range(OUT_OF_RANGE_MSG);
        }
        return operator[](idx);
    }

    /**
     * @brief Adds toAdd to the back of the VLVector.
     */
    void push_back(const T &toAdd)
    {
        if (_size == _capacity)
        {
            _increaseCapacity();
        }
        data()[_size++] = toAdd;
    }

    /**
     * @brief Inserts toAdd before iter.
     * @return An iter pointing to the new element.
     */
    iterator insert(const_iterator iter, const T &toAdd)
    {
        size_t inPlc = iter - cbegin();
        if (_size == _capacity)
        {
            _increaseCapacity();
        }
        const_iterator toCopy = cend();
        iterator copyTo = end();
        while (toCopy-- != (cbegin() + inPlc)) //copies all the items after the place to insert to, one space to the right
        {
            *(copyTo--) = *toCopy;
        }
        *copyTo = toAdd;
        _size++;
        return copyTo;
    }

    /**
     * @brief Adds all of the elements between first and last before iter.
     * @tparam InputIterator Iterator given by the user.
     * @return An iter pointing to the first element of the new ones.
     */
    template<class InputIterator>
    iterator
    insert(const_iterator iter, InputIterator const first, InputIterator const last)
    {
        size_t inPlc = iter - cbegin();
        const_iterator curPlc = iter;
        VLVector<T, StaticCapacity> after; //saves all of the objects to the right of the place to insert to.
        while (curPlc != cend())
        {
            after.push_back(*(curPlc++));
        }
        if ((_size += (last - first)) > _capacity)
        {
            _increaseCapacity();
        }
        iterator toIns = begin() + inPlc; //finds the place to start the insertion to.
        std::copy(first, last, toIns); //new items
        std::copy(after.begin(), after.end(), toIns + (last - first)); //old items to the right of the insertion place.
        return toIns;
    }

    /**
     * @brief Removes the last element of the container.
     * Doesn't call item destructor to avoid double destructor call upon decreaseCapacity, clear or destructor.
     */
    void pop_back()
    {
        --_size;
        if (_size <= StaticCapacity && _dynamic)
        {
            _decreaseCapacity();
        }
    }

    /**
     * @brief Erases The element that iter points to.
     * @return An iterator pointing to the next element.
     * Doesn't call item destructor to avoid double destructor call upon decreaseCapacity, clear or destructor.
     */
    iterator erase(const_iterator iter)
    {
        iterator insTo = begin(); //gets a non-const iterator to the same place as iter.
        while (insTo != iter)
        {
            ++insTo;
        }
        iterator first = insTo + NEXT_ELEM;
        std::copy(first, end(), insTo); //copies everything one space to the left.
        --_size;
        if (_size <= StaticCapacity && _dynamic)
        {
            _decreaseCapacity();
        }
        return insTo;
    }

    /**
     * @brief Erases all of the elements between first and last.
     * @return An iterator pointing to the next element.
     * Doesn't call item destructor to avoid double destructor call upon decreaseCapacity, clear or destructor.
     */
    iterator erase(const_iterator first, const_iterator last)
    {
        iterator copyTo = begin() + (first - cbegin()); //gets a non-const iterator to the same place as iter.
        std::copy(last, cend(), copyTo); //copies everything the desired amount of spaces to the left.
        _size -= last - first;
        if (_size <= StaticCapacity && _dynamic)
        {
            _decreaseCapacity();
        }
        return copyTo;
    }

    /**
     * @brief Deletes all of the elements in the container. Frees the array if dynamically allocated.
     */
    void clear() noexcept
    {
        if (_dynamic)
        {
            delete[] _curData;
            _curData = &(_statData[FIRST_IDX]);
            _dynamic = false;
        }
        _size = STARTING_SIZE;
    }

    /**
     * @return A pointer to the data structure holding the elements.
     */
    T *data() noexcept
    {
        return _curData;
    }

    /**
     * @return A pointer to the data structure holding the elements. const version.
     */
    const T *data() const noexcept
    {
        return _curData;
    }

    /**
     * @return The element at idx. Const version.
     */
    const T &operator[](size_t const &idx) const
    {
        return data()[idx];
    }

    /**
     * @return The element at idx.
     */
    T &operator[](size_t const &idx)
    {
        return data()[idx];
    }

    /**
     * @return true if all of the elements in both of the containers are equal. false otherwise.
     */
    bool operator==(VLVector const &toComp) const
    {
        return (size() == toComp.size() && std::equal(cbegin(), cend(), toComp.cbegin()));
    }

    /**
     * @return false if all of the elements in both of the containers are equal. true otherwise.
     */
    bool operator!=(VLVector const &toComp) const
    {
        return !((*this).operator==(toComp));
    }

    /**
     * @brief Assigns values equal to the values of rhs to the VLVector.
     * @return The assigned vector by ref.
     */
    VLVector<T, StaticCapacity> &operator=(VLVector const &rhs)
    {
        if (this != &rhs)
        {
            if (_dynamic)
            {
                delete[] _curData;
            }
            _capacity = rhs.capacity();
            if (_capacity > StaticCapacity)
            {
                _curData = new T[_capacity * sizeof(T)]();
                _dynamic = true;
            }
            insert(begin(), rhs.cbegin(), rhs.cend());
        }
        return *this;
    }

    /**
     * @return An iterator pointing to the first element of the container.
     */
    iterator begin() noexcept
    {
        return iterator(data());
    }

    /**
     * @return An iterator pointing to the address after the last element of the container.
     */
    iterator end() noexcept
    {
        return iterator(&(data()[_size]));
    }

    /**
     * @return A const_iterator pointing to the first element of the container.
     */
    const_iterator cbegin() const noexcept
    {
        return const_iterator(data());
    }

    /**
     * @return A const_iterator pointing to the address after the last element of the container.
     */
    const_iterator cend() const noexcept
    {
        return const_iterator(&(data()[_size]));
    }

    /**
     * @return A const_iterator pointing to the first element of the container.
     */
    const_iterator begin() const noexcept
    {
        return const_iterator(data());
    }

    /**
     * @return A const_iterator pointing to the address after the last element of the container.
     */
    const_iterator end() const noexcept
    {
        return const_iterator(&(data()[_size]));
    }
};


#endif //CPP_EXAM_VLVECTOR_HPP
