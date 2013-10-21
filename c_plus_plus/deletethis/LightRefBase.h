#ifndef _TEST_LIGHT_REFBASE_H
#define _TEST_LIGHT_REFBASE_H

namespace test
{
    template <class T>
    class LightRefBase
    {
        public:
            inline LightRefBase() : mCount(0) { }
            inline void incStrong(const void* id) const {
                mCount++;
            }
            inline void decStrong(const void* id) const {
                if (mCount-- == 1) {
                    delete static_cast<const T*>(this);
                }
            }
            //! DEBUGGING ONLY: Get current strong ref count.
            inline int getStrongCount() const {
                return mCount;
            }
            typedef LightRefBase<T> basetype;
            inline ~LightRefBase() { }
        private:
            mutable volatile int mCount;
    };

}
#endif
