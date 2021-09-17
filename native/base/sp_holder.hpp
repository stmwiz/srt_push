
#pragma once

#include <memory>

namespace xlab::base
{
    template <typename T>
    class SPHolder final
    {
    public:
        template <typename R>
        static SPHolder<T> *New(std::shared_ptr<R> &ptr) noexcept
        {
            static_assert(std::is_base_of<T, R>::value || std::is_same<T, R>::value, "ptr's type `R` not same `T` or not derived from `T`");
            if constexpr (std::is_same<T, R>::value)
            {
                return new SPHolder<T>(ptr);
            }
            else if constexpr (std::is_base_of<T, R>::value)
            {
                return new SPHolder<T>(std::static_pointer_cast<T>(ptr));
            }
            return nullptr;
        }

        template <typename R>
        static SPHolder<T> *New(std::shared_ptr<R> &&ptr) noexcept
        {
            static_assert(std::is_base_of<T, R>::value || std::is_same<T, R>::value, "ptr's type `R` not same `T` or not derived from `T`");
            if constexpr (std::is_same<T, R>::value)
            {
                return new SPHolder<T>(std::move(ptr));
            }
            else if constexpr (std::is_base_of<T, R>::value)
            {
                return new SPHolder<T>(std::move(std::static_pointer_cast<T>(ptr)));
            }
            return nullptr;
        }

        static void Delete(SPHolder<T> *holder) noexcept
        {
            delete holder;
        }

        static void DeleteFromHandler(int64_t handler) noexcept
        {
            delete SPHolder<T>::FromHandler(handler);
        }

    private:
        SPHolder(std::shared_ptr<T> &ptr) noexcept : mPtr(ptr) {}

        SPHolder(std::shared_ptr<T> &&ptr) noexcept : mPtr(std::move(ptr)) {}

        virtual ~SPHolder() noexcept
        {
            mPtr = nullptr;
        }

    public:
        static SPHolder<T> *FromHandler(int64_t handler) noexcept
        {
            return reinterpret_cast<SPHolder<T> *>(handler);
        }

        static std::shared_ptr<T> UnwrapFromHandler(int64_t handler) noexcept
        {
            auto holder = reinterpret_cast<SPHolder<T> *>(handler);
            if (!holder)
            {
                return nullptr;
            }
            return holder->Unwrap();
        }

        static int64_t NullHandler()
        {
            return 0;
        }

    private:
        SPHolder(const SPHolder &) = delete;

        SPHolder(SPHolder &&) = delete;

        SPHolder &operator=(SPHolder &) = delete;

        SPHolder &operator=(SPHolder &&) = delete;

    public:
        std::shared_ptr<T> &Unwrap() noexcept
        {
            return mPtr;
        }

        int64_t ToHandler()
        {
            return reinterpret_cast<int64_t>(this);
        }

    private:
        std::shared_ptr<T> mPtr;
    };

}