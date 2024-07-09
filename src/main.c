#include "../include/main.h"

void sig_handler(int signal);

int main()
{
	{
		SMART htable_o *global = orca(string_from("hello.orca"));
	}
	drop_globals();
	drop_ast_table();
	object_info_all();
	pool_clean();
	return 0;
}

/*
int main()
{
	{
		cache_restore();

		bumble_routes();
		user_routes();
		db_routes();

		SMART server_o *server = new_server(bumble, 1805, 1024, 3);
		if (is_error(server))
		{
			print(server);
			return 1;
		}

		accept_clients(server);
	}
	object_info_all();
	return 0;
}

__attribute__((destructor)) void after_main()
{
	drop_globals();
	printf("-----------object leaks------------\n");
	// object_info_all();
	pool_clean();
}

void sig_handler(int signal)
{
	drop_globals();
	perror("signal");
	exit(0);
}*/