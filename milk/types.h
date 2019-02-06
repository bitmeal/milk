namespace milk {

	enum t_type {
		s_int,
		s_fp,
		s_byte,
		s_bool,
		n_str,
		n_bin,
		t_map,
		t_list,
		undefined
	};

	enum iterator_type_t {
		map_i,
		list_i,
		scalar_i
	};

}