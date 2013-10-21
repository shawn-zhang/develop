#ifndef _SP_H_
#define _SP_H_

namespace test
{
    template <typename T>
    class sp
    {
        public:
            inline sp() : m_ptr(0) { }
            sp(T* other);
            ~sp();
        private:  
            template<typename Y> friend class sp;
            template<typename Y> friend class wp;
            void set_pointer(T* ptr);
            T* m_ptr;
    };

    template<typename T> sp<T>::sp(T* other) : m_ptr(other)
    {
        if (other) other->incStrong(this);
    }


    template<typename T>sp<T>::~sp()
    {
        if (m_ptr) m_ptr->decStrong(this);
    }
}

#endif
