#ifndef EXAMPLE_POLICY_H_
#define EXAMPLE_POLICY_H_

#include <boost/intrusive/options.hpp>
#include <boost/intrusive/list.hpp>

struct example_policy_traits
{
  static std::string GetName () { return "Example"; }
  
  struct policy_hook_type : public boost::intrusive::list_member_hook<> { int count; };

  template<class Container>
  struct container_hook { typedef boost::intrusive::member_hook< Container, policy_hook_type, &Container::policy_hook_ > type; };

  template<class Base, class Container, class Hook>
  struct policy 
  {
    typedef typename boost::intrusive::list< Container, Hook > policy_container;

    static int& get_count (typename Container::iterator item)
    {
      return static_cast<policy_hook_type*>
        (policy_container::value_traits::to_node_ptr(*item))->count;
    }

    static const int& get_count (typename Container::const_iterator item)
    {
      return static_cast<const policy_hook_type*>
        (policy_container::value_traits::to_node_ptr(*item))->count;
    }
    
    class type : public policy_container
    {
    public:
      typedef Container parent_trie;
    
      type (Base &base) : base_ (base), max_size_ (100), odd_ (true) { }

      inline void
      update (typename parent_trie::iterator item) { }
  
      inline bool
      insert (typename parent_trie::iterator item)
      {
        get_count (item) = 0;
        odd_ = ! odd_;
        if (odd_)
          {
            return false; // allow caching only every other packet
          }

        // use FIFO policy for others
        if (max_size_ != 0 && policy_container::size () >= max_size_)
          {
            base_.erase (&(*policy_container::begin ()));
          }
      
        policy_container::push_back (*item);
        return true;
      }
  
      inline void
      lookup (typename parent_trie::iterator item)
      {
        get_count (item) ++;
        if (get_count (item) > 2)
          {
            // do relocation
            policy_container::splice (policy_container::end (),
                                      *this,
                                      policy_container::s_iterator_to (*item));
          }
      }
  
      inline void erase (typename parent_trie::iterator item)
      {
        policy_container::erase (policy_container::s_iterator_to (*item));
      }

      inline void clear () { policy_container::clear (); }

      inline void set_max_size (size_t max_size) { max_size_ = max_size; }

      inline size_t get_max_size () const { return max_size_; }

    private:
      type () : base_(*((Base*)0)) { };

    private:
      Base &base_;
      size_t max_size_;
      bool odd_;
    };
  };
};

#endif
