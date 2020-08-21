#include "CBSHeuristic.h"
#include "CBS.h"
#include <ilcplex/ilocplex.h>


// integer linear programming for weighted vertex cover
// CPLEX
int CBSHeuristic::ILPForWMVC(const vector<int>& CG, const vector<int>& node_max_value)
{
	int N = (int)node_max_value.size();
	IloEnv env = IloEnv();
	IloModel model = IloModel(env);
	IloExpr sum_obj = IloExpr(env);
	IloNumVarArray var(env);
	IloRangeArray con(env);
	for (int i = 0; i < N; i++)
	{
		var.add(IloNumVar(env, 0, node_max_value[i] + 1, ILOINT));
		sum_obj += var[i];
	}
	model.add(IloMinimize(env, sum_obj));
	for (int i = 0; i < N; i++)
	{
		for (int j = i + 1; j < N; j++)
		{
			if (CG[i * N + j] > 0)
			{
				con.add(var[i] + var[j] >= CG[i * N + j]);
			}
		}
	}
	model.add(con);
	IloCplex cplex(env);
	double runtime = (double)(clock() - start_time) / CLOCKS_PER_SEC;
	cplex.setParam(IloCplex::TiLim, time_limit - runtime);
	int solution_cost = -1;
	cplex.extract(model);
	cplex.setOut(env.getNullStream());
	int rst = 0;
	if (cplex.solve())
		rst = (int)cplex.getObjValue();
	else
	{
		runtime = (double)(clock() - start_time) / CLOCKS_PER_SEC;
		if (time_limit > runtime + 1)
		{
			std::cout << "ERROR! remaining time = " << time_limit - runtime << " seconds" << endl;
			cplex.exportModel("error.lp");
			system("pause");
		}
	}
	env.end();
	return rst;
}
