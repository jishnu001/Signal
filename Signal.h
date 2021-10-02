#pragma once
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <any>
#include <typeinfo>

#define Connect(a, signal, b, slot) a.signal.connect(&b, &slot)
#define Disconnect(a, signal, b, slot) a.signal.disconnect(&b, &slot)

#define ConnectL(a, signal, slot) a.signal.connect(slot)
#define DisconnectL(a, signal, slot) a.signal.disconnect(slot)
namespace JSig
{
    class Slot_
    {
    public:
        Slot_() = default;
        virtual void emit() = 0;
        virtual ~Slot_() = default;
        virtual std::any get() const = 0;

    private:
    };

    template <class T, class F>
    class Slot : public Slot_
    {
    public:
        Slot(T *t, const F &f)
            : m_t(t), m_f(f)
        {
        }
        virtual ~Slot() = default;
        Slot(const Slot &) = default;
        Slot(Slot &&) = default;
        Slot &operator=(const Slot &) = default;
        Slot &operator=(Slot &&) = default;

        void emit() override
        {
            (m_t->*m_f)();
        }
        std::any get() const override
        {
            return std::make_any<T *>(m_t);
        }

    private:
        F m_f;
        T *m_t;
    };

    template <class F>
    class SlotL0 : public Slot_
    {
    public:
        SlotL0(const F &f)
            : m_f(f)
        {
        }
        virtual ~SlotL0() = default;
        SlotL0(const SlotL0 &) = default;
        SlotL0(SlotL0 &&) = default;
        SlotL0 &operator=(const SlotL0 &) = default;
        SlotL0 &operator=(SlotL0 &&) = default;

        void emit() override
        {
            (m_f)();
        }
        std::any get() const override
        {
            return std::string(typeid(m_f).name());
        }

    private:
        F m_f;
    };

    template <class Arg>
    class Slot1_
    {
    public:
        Slot1_(){};
        virtual void emit(Arg) = 0;
        virtual ~Slot1_(){};
        virtual std::any get() const = 0;

    private:
    };

    template <class T, class F, class Arg>
    class Slot1 : public Slot1_<Arg>
    {
    public:
        Slot1(T *t, const F &f)
            : m_t(t), m_f(f)
        {
        }
        virtual ~Slot1() = default;
        Slot1(const Slot1 &) = default;
        Slot1(Slot1 &&) = default;
        Slot1 &operator=(const Slot1 &) = default;
        Slot1 &operator=(Slot1 &&) = default;

        void emit(Arg arg) override
        {
            (m_t->*m_f)(arg);
        }
        std::any get() const override
        {
            return std::make_any<T *>(m_t);
        }

    private:
        F m_f;
        T *m_t;
    };

    template <class F, class Arg>
    class SlotL1 : public Slot1_<Arg>
    {
    public:
        SlotL1(const F &f)
            : m_f(f)
        {
        }
        virtual ~SlotL1() = default;
        SlotL1(const SlotL1 &) = default;
        SlotL1(SlotL1 &&) = default;
        SlotL1 &operator=(const SlotL1 &) = default;
        SlotL1 &operator=(SlotL1 &&) = default;

        void emit(Arg arg) override
        {
            (m_f)(arg);
        }
        std::any get() const override
        {
            return std::string(typeid(m_f).name());
        }

    private:
        F m_f;
    };

    class Signal
    {
    public:
        Signal() = default;
        template <class T, class F>
        void connect(T *t, const F &f)
        {
            std::lock_guard lock(mutex);
            slots.emplace_back(new Slot<T, F>(t, f));
        }
        template <class F>
        void connect(const F &f)
        {
            std::lock_guard lock(mutex);
            slots.emplace_back(new SlotL0<F>(f));
        }
        template <class T, class F>
        void disconnect(T *t, const F &f)
        {
            std::lock_guard lock(mutex);
            auto p = Slot<T, F>(t, f);
            auto it = std::find_if(slots.begin(), slots.end(), [&](const auto *l)
                                   {    
                                   if (!p.get().has_value() || !l->get().has_value())
                                   {
                                       return false;
                                   }
                                   return std::any_cast<T*>(p.get()) == std::any_cast<T*>(l->get()); });
            if (it != slots.end())
            {
                delete *it;
                slots.erase(it);
            }
        }
        template <class F>
        void disconnect(const F &f)
        {
            std::lock_guard lock(mutex);
            auto p = SlotL0<F>(f);
            auto it = std::find_if(slots.begin(), slots.end(), [&](const auto *l)
                                   {    
                                   if (!p.get().has_value() || !l->get().has_value())
                                   {
                                       return false;
                                   }
                                   return std::any_cast<std::string>(p.get()) == std::any_cast<std::string>(l->get()); });
            if (it != slots.end())
            {
                delete *it;
                slots.erase(it);
            }
        }
        void disconnectAll()
        {
            std::lock_guard lock(mutex);
            slots.clear();
        }
        void emit()
        {
            std::lock_guard lock(mutex);
            for (const auto &slot : slots)
            {
                auto fn = [=]()
                { slot->emit(); };
                std::thread t(fn);
                t.detach();
            }
        }

    private:
        std::mutex mutex;
        std::list<Slot_ *> slots;
    };

    template <class Arg>
    class Signal1
    {
    public:
        Signal1<Arg>(){};
        template <class T, class F>
        void connect(T *t, const F &f)
        {
            std::lock_guard lock(mutex);
            slots.emplace_back(new Slot1<T, F, Arg>(t, f));
        }
        template <class F>
        void connect(const F &f)
        {
            std::lock_guard lock(mutex);
            slots.emplace_back(new SlotL1<F, Arg>(f));
        }
        template <class T, class F>
        void disconnect(T *t, const F &f)
        {
            std::lock_guard lock(mutex);
            auto p = Slot1<T, F, Arg>(t, f);
            auto it = std::find_if(slots.begin(), slots.end(), [&](const auto *l)
                                   {    
                                   if (!p.get().has_value() || !l->get().has_value())
                                   {
                                       return false;
                                   }
                                   
                                   return std::any_cast<T*>(p.get()) == std::any_cast<T*>(l->get()); });
            if (it != slots.end())
            {
                delete *it;
                slots.erase(it);
            }
        }
        template <class F>
        void disconnect(const F &f)
        {
            std::lock_guard lock(mutex);
            auto p = SlotL1<F, Arg>(f);
            auto it = std::find_if(slots.begin(), slots.end(), [&](const auto *l)
                                   {    
                                   if (!p.get().has_value() || !l->get().has_value())
                                   {
                                       return false;
                                   }
                                   return std::any_cast<std::string>(p.get()) == std::any_cast<std::string>(l->get()); });
            if (it != slots.end())
            {
                delete *it;
                slots.erase(it);
            }
        }
        void disconnectAll()
        {
            std::lock_guard lock(mutex);
            slots.clear();
        }
        void emit(Arg arg)
        {
            std::lock_guard lock(mutex);
            for (const auto &slot : slots)
            {
                auto fn = [=]()
                { slot->emit(arg); };
                std::thread t(fn);
                t.detach();
            }
        }

    private:
        std::mutex mutex;
        std::list<Slot1_<Arg> *> slots;
    };
}