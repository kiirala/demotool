import java.io.*;
import java.lang.Math;
import java.util.Locale;

import org.apache.batik.bridge.BridgeContext;
import org.apache.batik.bridge.GVTBuilder;
import org.apache.batik.dom.svg.SAXSVGDocumentFactory;
import org.apache.batik.gvt.GraphicsNode;
import org.apache.batik.util.XMLResourceDescriptor;
import org.w3c.dom.Document;

/**
 * Ohjelma SVG-kuvien muuntamiseen yksinkertaisempaan muotoon. Tämä
 * yksinkertaisempi muoto sisältää ainoastaan kuvan reunaviivojen käyrät,
 * mikä on riittävästi kuvan piirtämiseen robotilla. Tuottaa jokaista
 * komentoriviparametrina annettua SVG-tiedostoa kohti samaan hakemistoon
 * .img -tiedoston.
 *
 * @author Niko Kiirala
 */
public class DrawSVG {
    public static void main(String[] args) throws Exception{
	if (args.length == 0) {
	    System.out.println("Syötä ainakin yksi tiedostonimi.");
	}
	else {
	    for (String name : args) {
		convertFile(name);
	    }
	}
    }

    private static void convertFile(String fname) throws Exception {
	String parser = XMLResourceDescriptor.getXMLParserClassName();
	//String parser = "org.xml.sax.helpers.ParserFactory";
	SAXSVGDocumentFactory f = new SAXSVGDocumentFactory(parser);
	File infile = new File(fname);
	File outfile = new File(fname.replace(".svg", ".h"));
	String uri = infile.toURI().toString();
	Document doc = f.createDocument(uri);
	GVTBuilder builder = new GVTBuilder();
	GraphicsNode imageRoot = builder.build(new BridgeContext(new SimpleUserAgent()), doc);

	Canvas canvas = new Canvas();
	imageRoot.paint(canvas);
	PrintStream os = new PrintStream(new FileOutputStream(outfile));

	/*
	float minx = 1e9f, maxx = -1e9f, miny = 1e9f, maxy = -1e9f;
	for (Canvas.Event e : canvas.events) {
	    for (int i = 0 ; i < 3 ; ++i) {
		if (e.pos[i * 2] < minx) minx = e.pos[i * 2];
		if (e.pos[i * 2] > maxx) maxx = e.pos[i * 2];
		if (e.pos[i * 2 + 1] < miny) miny = e.pos[i * 2 + 1];
		if (e.pos[i * 2 + 1] > maxy) maxy = e.pos[i * 2 + 1];
	    }
	}
	float scalex = 1, scaley = 1, scale = 1, shiftx = 0, shifty = 0;
	if (minx < 0) shiftx = -minx;
	if (miny < 0) shifty = -miny;
	if (maxx - minx > 170) scalex = 170 / (maxx - minx);
	if (maxy - miny > 380) scaley = 380 / (maxy - miny);
	scale = Math.min(scalex, scaley);
	*/

	Locale.setDefault(Locale.ROOT);
	String basename = infile.getName().replace(".svg", "");
	StringBuilder shape = new StringBuilder();
	os.printf("#ifndef DRAWSVG_IMAGE_%s\n", basename.toUpperCase());
	os.printf("#define DRAWSVG_IMAGE_%s\n", basename.toUpperCase());
	os.println();
	os.println("/*");
	os.println(" * Automatically generated file, do not edit.");
	os.printf(" * Generated from %s\n", infile.getName());
	os.println(" * Generator: DrawSVG.java, by Niko Kiirala");
	os.println("*/");
	os.println();	
	shape.append(String.format("static BezierShape drawsvg_%s[] = {\n", basename));
	int shape_num = 0;
	for (Canvas.MyShape s : canvas.shapes) {
	    shape.append(String.format("    {{%.3f, %.3f, %.3f, %.3f}, drawsvg_%s_%d},\n", s.colour[0], s.colour[1], s.colour[2], s.colour[3], basename, shape_num));
	    os.printf("static BezierPath drawsvg_%s_%d[] = {\n", basename, shape_num);
	    for (int i = 0 ; i < s.events.size() ; ++i) {
		Canvas.Event e = s.events.get(i);
		//System.out.println(e);
		if (i + 1 == s.events.size()) {
		    os.printf("    {{%.4f, %.4f, 0.0, 0.0, 0.0, 0.0}}\n",
			      e.pos[0], e.pos[1]);
		}
		else if (e.pos[2] == 0 && e.pos[3] == 0 &&
		    e.pos[4] == 0 && e.pos[5] == 0) {
		    Canvas.Event next = s.events.get(i + 1);
		    os.printf("    {{%.4f, %.4f, %.4f, %.4f, %.4f, %.4f}},\n",
			      e.pos[0], e.pos[1],
			      e.pos[0], e.pos[1],
			      next.pos[0], next.pos[1]);
		}
		else {
		    os.printf("    {{%.4f, %.4f, %.4f, %.4f, %.4f, %.4f}},\n",
			      e.pos[0], e.pos[1],
			      e.pos[2], e.pos[3],
			      e.pos[4], e.pos[5]);
		}
	    }
	    os.print("};\n\n");
	    ++shape_num;
	}
	os.print(shape);
	os.print("    {{0.0, 0.0, 0.0, 0.0}, 0}\n");
	os.print("};\n\n");
	os.printf("#endif /* DRAWSVG_IMAGE_%s */\n", basename.toUpperCase());
    }
}
