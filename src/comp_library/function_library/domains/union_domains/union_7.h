//-*-C++-*-

/* 
 * 	 This is a C++ file generated by write_union_domains.py 
 *
 * 	 author: Peter Staar  
 */ 


#ifndef UNION_7_H_
#define UNION_7_H_

#include "comp_library/function_library/domains/domain.h"   
#include "comp_library/type_list/type_list.h"   
#include "comp_library/type_list/type_list_definitions.h"   

using namespace TL; 


template< typename domain_0_t,
		  typename domain_1_t,
		  typename domain_2_t,
		  typename domain_3_t,
		  typename domain_4_t,
		  typename domain_5_t,
		  typename domain_6_t>
class union_7 : public domain 
{

public:
 
 	typedef typename domain_0_t::this_type domain_typelist_0;
	typedef typename domain_1_t::this_type domain_typelist_1;
	typedef typename domain_2_t::this_type domain_typelist_2;
	typedef typename domain_3_t::this_type domain_typelist_3;
	typedef typename domain_4_t::this_type domain_typelist_4;
	typedef typename domain_5_t::this_type domain_typelist_5;
	typedef typename domain_6_t::this_type domain_typelist_6;
	
	typedef typename Append<domain_typelist_0, typename Append<domain_typelist_1, typename Append<domain_typelist_2, typename Append<domain_typelist_3, typename Append<domain_typelist_4, typename Append<domain_typelist_5, domain_typelist_6>::Result >::Result >::Result >::Result >::Result >::Result  this_type;
 
 	union_7();
 
 	void reset(); 
 
 	int operator()(int l);

protected:

	domain_0_t domain_0;
	domain_1_t domain_1;
	domain_2_t domain_2;
	domain_3_t domain_3;
	domain_4_t domain_4;
	domain_5_t domain_5;
	domain_6_t domain_6;

};



template< typename domain_0_t,
		  typename domain_1_t,
		  typename domain_2_t,
		  typename domain_3_t,
		  typename domain_4_t,
		  typename domain_5_t,
		  typename domain_6_t>
union_7<domain_0_t, domain_1_t, domain_2_t, domain_3_t, domain_4_t, domain_5_t, domain_6_t>::union_7():
	domain(),
	domain_0(),
	domain_1(),
	domain_2(),
	domain_3(),
	domain_4(),
	domain_5(),
	domain_6()
{

	size = domain_0.get_size() + domain_1.get_size() + domain_2.get_size() + domain_3.get_size() + domain_4.get_size() + domain_5.get_size() + domain_6.get_size();
	
	branch_domain_sizes.push_back(size);
	leaf_domain_sizes.push_back(size);
}

template< typename domain_0_t,
		  typename domain_1_t,
		  typename domain_2_t,
		  typename domain_3_t,
		  typename domain_4_t,
		  typename domain_5_t,
		  typename domain_6_t>
void union_7<domain_0_t, domain_1_t, domain_2_t, domain_3_t, domain_4_t, domain_5_t, domain_6_t>::reset()
{
domain::reset();

domain_0.reset();
domain_1.reset();
domain_2.reset();
domain_3.reset();
domain_4.reset();
domain_5.reset();
domain_6.reset();


	size = domain_0.get_size() + domain_1.get_size() + domain_2.get_size() + domain_3.get_size() + domain_4.get_size() + domain_5.get_size() + domain_6.get_size();
	
branch_domain_sizes.resize(0);
branch_domain_sizes.push_back(size);

leaf_domain_sizes.resize(0);
leaf_domain_sizes.push_back(size);
}

template< typename domain_0_t,
		  typename domain_1_t,
		  typename domain_2_t,
		  typename domain_3_t,
		  typename domain_4_t,
		  typename domain_5_t,
		  typename domain_6_t>
int union_7<domain_0_t, domain_1_t, domain_2_t, domain_3_t, domain_4_t, domain_5_t, domain_6_t>::operator()(int l)
{
	 assert(l>=0 && l<size);
	 return l;
}

#endif
