#pragma once
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>
// A — Allocation manuelle via allocator : on sépare mémoire brute et objets construits.
// Aucun besoin de constructeur par défaut pour T. Version pédagogique, sans allocator custom.
template<class T> class TableauDynamique {
    using A = std::allocator<T>;
    using Traits = std::allocator_traits<A>;
    A alloc_;
    T* data_ = nullptr;
    std::size_t size_ = 0, capacity_ = 0;
    void liberer() noexcept {
        clear();
        if (data_) Traits::deallocate(alloc_, data_, capacity_);
        data_ = nullptr; capacity_ = 0;
    }
public:
    TableauDynamique() = default;
    ~TableauDynamique() { liberer(); } // RAII : un propriétaire, une libération.
    TableauDynamique(const TableauDynamique& other) {
        // Si une copie de T échoue, le destructeur de CE tableau ne sera pas appelé.
        // On nettoie donc explicitement les objets déjà construits.
        try { for (std::size_t i=0; i<other.size_; ++i) push_back(other.data_[i]); }
        catch (...) { liberer(); throw; }
    }
    TableauDynamique(TableauDynamique&& other) noexcept { swap(other); }
    // Copy-and-swap : copie d'abord, engagement ensuite. Gère aussi l'auto-affectation.
    TableauDynamique& operator=(const TableauDynamique& other) {
        TableauDynamique copie(other); swap(copie); return *this;
    }
    TableauDynamique& operator=(TableauDynamique&& other) noexcept {
        if (this != &other) { liberer(); swap(other); }
        return *this;
    }
    void swap(TableauDynamique& other) noexcept {
        std::swap(data_,other.data_); std::swap(size_,other.size_);
        std::swap(capacity_,other.capacity_);
    }
    // Le paramètre par valeur protège aussi push_back(at(0)) lors d'une réallocation.
    void push_back(T value) {
        if (size_ < capacity_) {
            Traits::construct(alloc_,data_+size_,std::move(value)); ++size_; return;
        }
        const auto max = Traits::max_size(alloc_);
        if (capacity_ == max) throw std::length_error("Tableau trop grand");
        const auto next = capacity_ == 0 ? 1 : (capacity_ > max/2 ? max : capacity_*2);
        T* fresh = Traits::allocate(alloc_, next);
        std::size_t copied = 0;
        bool last = false;
        try {
            // Construire le nouvel élément avant de déplacer les anciens.
            Traits::construct(alloc_,fresh+size_,std::move(value)); last=true;
            for (; copied<size_; ++copied)
                Traits::construct(alloc_,fresh+copied,std::move_if_noexcept(data_[copied]));
        } catch (...) {
            for (std::size_t i=0;i<copied;++i) Traits::destroy(alloc_,fresh+i);
            if(last) Traits::destroy(alloc_,fresh+size_);
            Traits::deallocate(alloc_,fresh,next); throw;
        }
        // Garantie forte si T est copiable ou déplaçable sans exception.
        // Pour un T uniquement déplaçable dont le move lève, les anciens T peuvent changer.
        const auto oldSize=size_;
        liberer(); data_=fresh; capacity_=next; size_=oldSize+1;
    }
    void pop_back() {
        if(size_==0) throw std::out_of_range("pop_back sur tableau vide");
        Traits::destroy(alloc_,data_+ --size_);
    }
    T& at(std::size_t i) {
        if(i>=size_) throw std::out_of_range("Indice invalide");
        return data_[i];
    }
    const T& at(std::size_t i) const {
        if(i>=size_) throw std::out_of_range("Indice invalide");
        return data_[i];
    }
    std::size_t size() const noexcept { return size_; }
    void clear() noexcept { while(size_) Traits::destroy(alloc_,data_+ --size_); }
};
