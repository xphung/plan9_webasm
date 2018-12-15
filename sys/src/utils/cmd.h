#define TOKCAT(x,y)	x ## _ ## y
#define TOKEXPAND(x,y)	TOKCAT(x,y)

#define aflag TOKEXPAND(main, a_flag)
#define bflag TOKEXPAND(main, b_flag)
#define cflag TOKEXPAND(main, c_flag)
#define dflag TOKEXPAND(main, d_flag)
#define eflag TOKEXPAND(main, e_flag)
#define fflag TOKEXPAND(main, f_flag)
#define gflag TOKEXPAND(main, g_flag)
#define hflag TOKEXPAND(main, h_flag)
#define iflag TOKEXPAND(main, i_flag)
#define jflag TOKEXPAND(main, j_flag)
#define kflag TOKEXPAND(main, k_flag)
#define lflag TOKEXPAND(main, l_flag)
#define mflag TOKEXPAND(main, m_flag)
#define nflag TOKEXPAND(main, n_flag)
#define oflag TOKEXPAND(main, o_flag)
#define pflag TOKEXPAND(main, p_flag)
#define qflag TOKEXPAND(main, q_flag)
#define rflag TOKEXPAND(main, r_flag)
#define sflag TOKEXPAND(main, s_flag)
#define tflag TOKEXPAND(main, t_flag)
#define uflag TOKEXPAND(main, u_flag)
#define vflag TOKEXPAND(main, v_flag)
#define wflag TOKEXPAND(main, w_flag)
#define xflag TOKEXPAND(main, x_flag)
#define yflag TOKEXPAND(main, y_flag)
#define zflag TOKEXPAND(main, z_flag)
#define Fflag TOKEXPAND(main, F_flag)
#define Tflag TOKEXPAND(main, T_flag)
#include cmd_source
#undef main
#undef cmd_source
#undef TOKEXPAND

