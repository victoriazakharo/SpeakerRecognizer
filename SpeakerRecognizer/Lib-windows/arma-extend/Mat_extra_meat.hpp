template<typename eT>
inline
Mat<eT>::Mat(const std::vector< std::vector<eT> >& list)
	: n_rows(0)
	, n_cols(0)
	, n_elem(0)
	, vec_state(0)
	, mem_state(0)
	, mem()
{
	arma_extra_debug_sigprint();

	uword x_n_rows = uword(list[0].size());
	uword x_n_cols = uword(list.size());

	Mat<eT>& t = (*this);

	if (t.mem_state == 3)
	{
		arma_debug_check(((x_n_rows != t.n_rows) || (x_n_cols != t.n_cols)), "Mat::init(): size mismatch between fixed size matrix and initialiser list");
	}
	else
	{
		t.set_size(x_n_rows, x_n_cols);
	}

	for (int i = 0; i <x_n_rows; i++)
	{
		for (int j = 0; j <x_n_cols; j++)
		{
			t.at(i, j) = list[j][i];
		}
	}
}

