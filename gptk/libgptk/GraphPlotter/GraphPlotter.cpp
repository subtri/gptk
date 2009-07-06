#include "GraphPlotter.h"

using namespace std;
using namespace itpp;

GraphPlotter::GraphPlotter()
{
	plotterStream = popen("gnuplot -persist -background white","w");
	if (!plotterStream)
	{
		cerr << "Couldn't open connection to gnuplot" << endl;
		return;
	}
}


GraphPlotter::~GraphPlotter()
{
	clearPlot();
	if (pclose(plotterStream) == -1)
	{
		cerr << "Problem closing communication to gnuplot" << endl;
		return;
	}
}



void GraphPlotter::clearPlot()
{      
	if (tempList.size() > 0)
	{
		for (unsigned int i = 0; i < tempList.size(); i++)
		{
			remove(tempList[i].c_str());
		}
		tempList.clear();
	}
}

void GraphPlotter::sendCommand(const char *command, ...)
{
	int MAX_BUFFER = 4096;
	va_list ap;
	char cmd[MAX_BUFFER];
	va_start(ap, command);
	vsprintf(cmd, command, ap);
	va_end(ap);
	strcat(cmd, "\n");
	fputs(cmd, plotterStream);
	fflush(plotterStream);
	sleep(1);
}

void GraphPlotter::setYLabel(const string label)
{
	ostringstream cmdstr;
	cmdstr << "set xlabel \"" << label << "\"";
	sendCommand(cmdstr.str().c_str());
}

void GraphPlotter::setXLabel(const string label)
{
	ostringstream cmdstr;
	cmdstr << "set xlabel \"" << label << "\"";
	sendCommand(cmdstr.str().c_str());
}
    
void GraphPlotter::plotPoints(const vec &x, const vec &y, const string title, const pointStyle ps, const plotColor pc)
{
	ofstream tmp;
	ostringstream cmdstr;
	char name[] = "/tmp/scratchGraphPlotterXXXXXX";
	string seriesTitle = title;

	if(x.size() != y.size())
	{
		return;
	}

	if(mkstemp(name) == -1)
	{
		cerr << "Cannot create temporary file: exiting plot" << endl;
		return;
	}

	tmp.open(name);
	if(tmp.bad())
	{
		cerr << "Cannot create temorary file: exiting plot" << endl;
		return;
	}

	tempList.push_back(name);

	for(int i = 0; i < x.size(); i++)
	{
		tmp << x(i) << " " << y(i) << endl;
	}
	tmp.flush();    
	tmp.close();

	if(tempList.size() > 1)
	{
		cmdstr << "replot ";
	}
	else
	{
		cmdstr << "plot ";
	}

	if(title == "")
	{
		seriesTitle = "untitled";
	}

	cmdstr << "\"" << name << "\" title \"" << seriesTitle << "\" with ";

	if(ps == LINE)
	{
		cmdstr << "lines" << " lt " << pc;
	}
	else
	{
		cmdstr << "points pt " << ps << " lt " << pc;
	}

	sendCommand(cmdstr.str().c_str());
}

