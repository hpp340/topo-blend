#pragma once
#include "DynamicGraph.h"

#include <QFile>
#include <QXmlStreamWriter>

// Save to XML based graph file
static void toGraphML(const DynamicGraph & g, QString fileName = "mygraph")
{
	QFile file(fileName + ".graphml");
	if (!file.open(QFile::WriteOnly | QFile::Text))	return;

	QXmlStreamWriter xml(&file);
	xml.setAutoFormatting(true);

	xml.writeStartDocument();
	xml.writeStartElement("graphml");
	xml.writeAttribute("xmlns","http://graphml.graphdrawing.org/xmlns");

	// Write all graph keys

	// Node keys
	xml.writeStartElement("key");
	xml.writeAttribute("attr.name","label");xml.writeAttribute("attr.type","string");
	xml.writeAttribute("for","node");xml.writeAttribute("id","label");
	xml.writeEndElement();

	xml.writeStartElement("key");
	xml.writeAttribute("attr.name","x");xml.writeAttribute("attr.type","double");
	xml.writeAttribute("for","node");xml.writeAttribute("id","x");
	xml.writeEndElement();

	xml.writeStartElement("key");
	xml.writeAttribute("attr.name","y");xml.writeAttribute("attr.type","double");
	xml.writeAttribute("for","node");xml.writeAttribute("id","y");
	xml.writeEndElement();

	xml.writeStartElement("key");
	xml.writeAttribute("attr.name","size");xml.writeAttribute("attr.type","double");
	xml.writeAttribute("for","node");xml.writeAttribute("id","size");
	xml.writeEndElement();

	// Special node keys
	xml.writeStartElement("key");
	xml.writeAttribute("attr.name","Original");xml.writeAttribute("attr.type","string");
	xml.writeAttribute("for","node");xml.writeAttribute("id","original");
	xml.writeEndElement();

	xml.writeStartElement("key");
	xml.writeAttribute("attr.name","State");xml.writeAttribute("attr.type","int");
	xml.writeAttribute("for","node");xml.writeAttribute("id","state");
	xml.writeEndElement();

	xml.writeStartElement("key");
	xml.writeAttribute("attr.name","Correspond");xml.writeAttribute("attr.type","string");
	xml.writeAttribute("for","node");xml.writeAttribute("id","correspond");
	xml.writeEndElement();

	// Edge keys
	xml.writeStartElement("key");
	xml.writeAttribute("attr.name","Edge Label");xml.writeAttribute("attr.type","string");
	xml.writeAttribute("for","edge");xml.writeAttribute("id","edgelabel");
	xml.writeEndElement();

	xml.writeStartElement("key");
	xml.writeAttribute("attr.name","Edge Id");xml.writeAttribute("attr.type","string");
	xml.writeAttribute("for","edge");xml.writeAttribute("id","edgeid");
	xml.writeEndElement();

	xml.writeStartElement("key");
	xml.writeAttribute("attr.name","weight");xml.writeAttribute("attr.type","double");
	xml.writeAttribute("for","edge");xml.writeAttribute("id","weight");
	xml.writeEndElement();

	// Write the graph data
	xml.writeStartElement("graph");
	xml.writeAttribute("edgedefault","undirected");

	// Place on a grid
	double size = 50;
	double spacing = size / 10.0;
	double x = 0, y = 0;
	double dx = size + spacing;
	double dy = dx;
	int length = sqrt((double)g.nodes.size());

	// Write nodes
	foreach(int i, g.nodes.keys())
	{
		const SimpleNode & n = g.nodes[i];

		xml.writeStartElement("node");
		xml.writeAttribute("id", QString::number(n.idx));

		// Original node label
		xml.writeStartElement("data");
		xml.writeAttribute("key","label");
		xml.writeCharacters(n.property["original"].toString());
		xml.writeEndElement();

		xml.writeStartElement("data");
		xml.writeAttribute("key","original");
		xml.writeCharacters(n.property["original"].toString());
		xml.writeEndElement();

		// Special keys
		if(n.property.contains("state"))
		{
			xml.writeStartElement("data");
			xml.writeAttribute("key","state");
			xml.writeCharacters(n.property["state"].toString());
			xml.writeEndElement();
		}
		if(n.property.contains("correspond"))
		{
			xml.writeStartElement("data");
			xml.writeAttribute("key","correspond");
			xml.writeCharacters(n.property["correspond"].toString());
			xml.writeEndElement();
		}

		// Position
		xml.writeStartElement("data");
		xml.writeAttribute("key","x");
		xml.writeCharacters( QString::number(x) );
		xml.writeEndElement();

		xml.writeStartElement("data");
		xml.writeAttribute("key","y");
		xml.writeCharacters( QString::number(y) );
		xml.writeEndElement();

		xml.writeStartElement("data");
		xml.writeAttribute("key","size");
		xml.writeCharacters( QString::number(size * 0.5) );
		xml.writeEndElement();

		// Move virtual cursor
		x += dx;
		if(i % length == 0){
			x = 0;
			y += dy;
		}

		xml.writeEndElement();
	}

	// Write edges
	foreach(int i, g.edges.keys())
	{
		const SimpleEdge & e = g.edges[i];

		xml.writeStartElement("edge");
		xml.writeAttribute("source", QString::number(g.nodes[e.n[0]].idx));
		xml.writeAttribute("target", QString::number(g.nodes[e.n[1]].idx));

		// Edge weight
		xml.writeStartElement("data");
		xml.writeAttribute("key","weight");
		xml.writeCharacters("1.0");
		xml.writeEndElement();

		xml.writeEndElement();
	}

	xml.writeEndElement();
	xml.writeEndDocument();

	file.close();
}

// Save to XML based graph file
static void toGraphviz(const DynamicGraph & g, QString fileName = "mygraph", bool isOutputImage = true, QString subcaption="", QString caption = "")
{
	QFile file(fileName + ".gv");
	if (!file.open(QFile::WriteOnly | QFile::Text))	return;
	QTextStream out(&file);

	out << "graph G{\n";
	out << "\t" << "node [ fontcolor = black, color = white, style = filled ];" << "\n";

	// Place on a grid
	double size = 50;
	double spacing = size / 10.0;
	double x = 0, y = 0;
	double dx = size + spacing;
	double dy = dx;
	int length = sqrt((double)g.nodes.size());

	QStringList colors;

	colors << "red" << "green" << "blue" << "orangered" << "greenyellow" << "navy";

	// Write nodes
	foreach(int i, g.nodes.keys())
	{
		const SimpleNode & n = g.nodes[i];

		out << "\t" << QString("%1 [label = \"%2\", color = %3];").arg(n.idx).arg(n.property["original"].toString()).arg(colors[n.property["state"].toInt()]) << "\n";

		// Move virtual cursor
		x += dx;
		if(i % length == 0){
			x = 0;
			y += dy;
		}
	}

	// Write edges
	foreach(int i, g.edges.keys())
	{
		const SimpleEdge & e = g.edges[i];

		const SimpleNode & n1 = g.nodes[e.n[0]];
		const SimpleNode & n2 = g.nodes[e.n[1]];

		out << "\t\"" << n1.idx << "\" -- \"" << n2.idx << "\";\n";
	}

	// Labels
	out << "label = \"\\n\\n" << caption << "\\n" << subcaption << "\"\n";
	out << "fontsize = 20;\n";

	out << "}\n";

	file.close();

	// Show image of graph (assuming Graphviz is installed)
	if(isOutputImage)
	{
		system(qPrintable(QString("dot %1 -Tpng > %2").arg(fileName+".gv").arg(fileName+".png")));
	}
}