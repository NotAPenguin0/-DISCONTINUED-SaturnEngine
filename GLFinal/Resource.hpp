#ifndef SATURN_RESOURCE_HPP_
#define SATURN_RESOURCE_HPP_

#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>

namespace Saturn {

struct RefCount {
    RefCount() : count(0) {}
    int count;
};

template<class R, class K = std::string>
class ResourceManager;

template<class R>
class ResourceRef {
public:
    using type = R;
    using reference = R&;
    using const_reference = R const&;
    using pointer = R*;
    using const_pointer = R const*;

    using manager = ResourceManager<R>;

    ResourceRef() {
        man = nullptr;
        res = nullptr;
        refs = nullptr;
    }

    ResourceRef(ResourceRef const& rhs) {
        rhs.refs->count++;

        man = rhs.man;
        res = rhs.res;
        refs = rhs.refs;
    }

    ResourceRef& operator=(ResourceRef const& rhs) {
        if (res != nullptr) { refs->count--; }
        if (rhs.res == nullptr) return *this;

        rhs.refs->count++;

        man = rhs.man;
        res = rhs.res;
        refs = rhs.refs;

        return *this;
    }

    ~ResourceRef() {
        if (res != nullptr) refs->count--;
    }

    pointer operator->() {
        if (res == nullptr) throw std::exception("No resource held");

        return res;
    }

    const_pointer operator->() const {
        if (res == nullptr) throw std::exception("No resource held");
        return res;
    }

    reference get() {
        if (res == nullptr) throw std::exception("No resource held");
        return *res;
    }
    const_reference get() const {
        if (res == nullptr) throw std::exception("No resource held");
        return *res;
    }

    reference operator*() {
        if (res == nullptr) throw std::exception("No resource held");
        return get();
    }
    reference operator*() const {
        if (res == nullptr) throw std::exception("No resource held");
        return get();
    }

private:
    template<class Res, class Key>
    friend class ResourceManager;

    ResourceRef(pointer r, RefCount* count, manager* m) :
        res(r), man(m), refs(count) {
        refs->count++;
    }

    pointer res;
    manager* man;
    RefCount* refs;
};

template<class R, class Key>
class ResourceManager {
public:
    template<class F, class G>
    ResourceManager(F&& load_func, G&& unload_func) :
        load_function(load_func), unload_function(unload_func) {}

    ResourceRef<R> get(Key const& k) {
        // Try-catch block to test if resource is already loaded
        try {
            auto& r = resources.at(k);
            return ResourceRef<R>(&r.first, &r.second, this);
        } catch (...) // Resource does not yet exist
        {
            R r;
            if (load_function(k, r)) {
                resources.emplace(k, std::pair<R, RefCount>{std::move(r), RefCount{}});
                auto& res = resources.at(k);
                return ResourceRef<R>(&res.first, &res.second, this);
            } else {
                throw std::runtime_error("Failed to load resource with key: " +
                                         std::string(k));
            }
        }
    }

    ~ResourceManager() {
        for (auto& [key, val] : resources) {
            auto& res = val.first;
            unload_function(res);
        }
    }

private:
    std::unordered_map<Key, std::pair<R, RefCount>> resources;
    std::function<bool(Key, R&)> load_function;
    std::function<void(R&)> unload_function;
};

} // namespace Saturn

#endif
