import java.awt.*;
import java.awt.RenderingHints.Key;
import java.awt.font.FontRenderContext;
import java.awt.font.GlyphVector;
import java.awt.geom.AffineTransform;
import java.awt.geom.PathIterator;
import java.awt.image.*;
import java.awt.image.renderable.RenderableImage;
import java.text.AttributedCharacterIterator;
import java.util.*;

/**
 * Piirtoalue, joka tallettaa annetut piirtokomennot, jotta niistä voidaan
 * tuottaa yksinkertaisessa muodossa oleva tiedosto.
 *
 * @author Niko Kiirala
 */
public class Canvas extends Graphics2D {
    /**
     * Piirtotapahtuman tiedot. Tallettaa uuden käyrän kolme mahdollista
     * ohjauspistettä vastaavat 6 lukua ja tiedon, väritetäänkö tämä
     * käyrä.
     */
    class Event {
	public float[] pos;
	public Event(float[] pos) {
	    this.pos = Arrays.copyOf(pos, 6);
	}
	public String toString() {
	    return String.format("%8.2f %8.2f %8.2f %8.2f %8.2f %8.2f",
				 pos[0], pos[1], pos[2], pos[3],
				 pos[4], pos[5]);
	}
    }

    class MyShape {
	/** Talletetut tapahtumat. */
	public ArrayList<Event> events;
	public float[] colour;

	public MyShape(ArrayList<Event> events, float[] colour) {
	    this.events = events;
	    this.colour = Arrays.copyOf(colour, 4);
	}
    }

    /** Tämänhetkinen siirrosmatriisi. */
    AffineTransform trans;
    public ArrayList<MyShape> shapes;
    float[] currentColour;

    public Canvas() {
	super();
	trans = new AffineTransform();
	shapes = new ArrayList<MyShape>();
	currentColour = new float[4];
    }
	
    @Override
	public void addRenderingHints(Map<?, ?> hints) {
	// TODO Auto-generated method stub

    }

    @Override
	public void clip(Shape s) {
	// TODO Auto-generated method stub

    }

    /**
     * Tallettaa annetun kuvion käyrät tapahtumiin. Oleellisin osa tämän
     * luokan toiminnallisuutta.
     */
    @Override public void draw(Shape s) {
	PathIterator path = s.getPathIterator(trans);
	float[] start = null;
	float[] pos = new float[6];
	float[] substart = null;
	float[] currentPoint = new float[2];
	int prevType = 0;
	ArrayList<Event> events = new ArrayList<Event>();
	while (!path.isDone()) {
	    Arrays.fill(pos, 0);
	    int type = path.currentSegment(pos);
	    prevType = type;
	    if (start == null) {
		start = Arrays.copyOf(pos, 6);
		substart = Arrays.copyOf(pos, 6);
	    }
	    if (type == PathIterator.SEG_MOVETO) {
		substart = Arrays.copyOf(pos, 6);
		currentPoint = Arrays.copyOf(pos, 2);
		if (events.size() > 0) {
		    shapes.add(new MyShape(events, currentColour));
		    events = new ArrayList<Event>();
		}
	    }
	    else if (type == PathIterator.SEG_CLOSE) {
		events.add(new Event(substart));
		currentPoint = Arrays.copyOf(substart, 2);
	    }
	    else {
		float[] p = new float[6];
		p[0] = currentPoint[0];
		p[1] = currentPoint[1];
		if (type == PathIterator.SEG_LINETO) {
		    p[2] = p[3] = p[4] = p[5] = 0.0f;
		    currentPoint[0] = pos[0];
		    currentPoint[1] = pos[1];
		}
		else if (type == PathIterator.SEG_QUADTO) {
		    p[2] = p[4] = pos[0];
		    p[3] = p[5] = pos[1];
		    currentPoint[0] = pos[2];
		    currentPoint[1] = pos[3];		    
		}
		else {
		    p[2] = pos[0];
		    p[3] = pos[1];
		    p[4] = pos[2];
		    p[5] = pos[3];
		    currentPoint[0] = pos[4];
		    currentPoint[1] = pos[5];		    
		}
		events.add(new Event(p));
	    }
	    path.next();
	}
	if (prevType != PathIterator.SEG_CLOSE) {
	    events.add(new Event(substart));
	}
	if (events.size() > 0) {
	    shapes.add(new MyShape(events, currentColour));
	}
    }

    @Override
	public void drawGlyphVector(GlyphVector g, float x, float y) {
	// TODO Auto-generated method stub

    }

    @Override
	public boolean drawImage(Image img, AffineTransform xform, ImageObserver obs) {
	// TODO Auto-generated method stub
	return false;
    }

    @Override
	public void drawImage(BufferedImage img, BufferedImageOp op, int x, int y) {
	// TODO Auto-generated method stub

    }

    @Override
	public void drawRenderableImage(RenderableImage img, AffineTransform xform) {
	// TODO Auto-generated method stub

    }

    @Override
	public void drawRenderedImage(RenderedImage img, AffineTransform xform) {
	// TODO Auto-generated method stub

    }

    @Override
	public void drawString(String str, int x, int y) {
	// TODO Auto-generated method stub

    }

    @Override
	public void drawString(String str, float x, float y) {
	// TODO Auto-generated method stub

    }

    @Override
	public void drawString(AttributedCharacterIterator iterator, int x, int y) {
	// TODO Auto-generated method stub

    }

    @Override
	public void drawString(AttributedCharacterIterator iterator, float x,
			       float y) {
	// TODO Auto-generated method stub

    }

    /**
     * Piirtää annetun kuvion ääriviivan. Kuvioiden täyttäminen ei ole
     * tällä hetkellä tuettua.
     */
    @Override public void fill(Shape s) {
	draw(s);
    }

    @Override
	public Color getBackground() {
	// TODO Auto-generated method stub
	return null;
    }

    @Override
	public Composite getComposite() {
	// TODO Auto-generated method stub
	return null;
    }

    @Override
	public GraphicsConfiguration getDeviceConfiguration() {
	// TODO Auto-generated method stub
	return null;
    }

    @Override
	public FontRenderContext getFontRenderContext() {
	// TODO Auto-generated method stub
	return null;
    }

    @Override
	public Paint getPaint() {
	// TODO Auto-generated method stub
	return null;
    }

    @Override
	public Object getRenderingHint(Key hintKey) {
	// TODO Auto-generated method stub
	return null;
    }

    @Override
	public RenderingHints getRenderingHints() {
	// TODO Auto-generated method stub
	return null;
    }

    @Override
	public Stroke getStroke() {
	// TODO Auto-generated method stub
	return null;
    }

    @Override public AffineTransform getTransform() {
	return (AffineTransform)trans.clone();
    }

    @Override
	public boolean hit(Rectangle rect, Shape s, boolean onStroke) {
	// TODO Auto-generated method stub
	return false;
    }

    @Override public void rotate(double theta) {
	trans.rotate(theta);
    }

    @Override public void rotate(double theta, double x, double y) {
	trans.rotate(theta, x, y);
    }

    @Override public void scale(double sx, double sy) {
	trans.scale(sx, sy);
    }

    @Override
	public void setBackground(Color color) {
	// TODO Auto-generated method stub

    }

    @Override
	public void setComposite(Composite comp) {
	// TODO Auto-generated method stub

    }

    @Override public void setPaint(Paint paint) {
	if (paint instanceof Color)
	    setColor((Color)paint);
	else
	    System.out.println("Unimplemented paint type: " + paint);
    }

    @Override
	public void setRenderingHint(Key hintKey, Object hintValue) {
	// TODO Auto-generated method stub

    }

    @Override
	public void setRenderingHints(Map<?, ?> hints) {
	// TODO Auto-generated method stub

    }

    @Override
	public void setStroke(Stroke s) {
	// TODO Auto-generated method stub

    }

    @Override public void setTransform(AffineTransform Tx) {
	trans = (AffineTransform)Tx.clone();
    }

    @Override public void shear(double shx, double shy) {
	trans.shear(shx, shy);
    }

    @Override public void transform(AffineTransform Tx) {
	trans.concatenate(Tx);
    }

    @Override public void translate(int x, int y) {
	trans.translate(x, y);
    }

    @Override public void translate(double tx, double ty) {
	trans.translate(tx, ty);
    }

    @Override
	public void clearRect(int x, int y, int width, int height) {
	// TODO Auto-generated method stub

    }

    @Override
	public void clipRect(int x, int y, int width, int height) {
	// TODO Auto-generated method stub

    }

    @Override
	public void copyArea(int x, int y, int width, int height, int dx, int dy) {
	// TODO Auto-generated method stub

    }

    /*@SuppressWarnings("unchecked")*/ @Override public Graphics create() {
	Canvas c = new Canvas();
	c.trans = (AffineTransform) this.trans.clone();
	for (MyShape s : this.shapes) {
	    c.shapes.add(s);
	}
	c.currentColour = Arrays.copyOf(currentColour, 4);
	return c;
    }

    @Override public void dispose() {
	// Ei varattuja resursseja, joita tarvitsisi vapauttaa
    }

    @Override
	public void drawArc(int x, int y, int width, int height, int startAngle,
			    int arcAngle) {
	// TODO Auto-generated method stub

    }

    @Override
	public boolean drawImage(Image img, int x, int y, ImageObserver observer) {
	// TODO Auto-generated method stub
	return false;
    }

    @Override
	public boolean drawImage(Image img, int x, int y, Color bgcolor,
				 ImageObserver observer) {
	// TODO Auto-generated method stub
	return false;
    }

    @Override
	public boolean drawImage(Image img, int x, int y, int width, int height,
				 ImageObserver observer) {
	// TODO Auto-generated method stub
	return false;
    }

    @Override
	public boolean drawImage(Image img, int x, int y, int width, int height,
				 Color bgcolor, ImageObserver observer) {
	// TODO Auto-generated method stub
	return false;
    }

    @Override
	public boolean drawImage(Image img, int dx1, int dy1, int dx2, int dy2,
				 int sx1, int sy1, int sx2, int sy2, ImageObserver observer) {
	// TODO Auto-generated method stub
	return false;
    }

    @Override
	public boolean drawImage(Image img, int dx1, int dy1, int dx2, int dy2,
				 int sx1, int sy1, int sx2, int sy2, Color bgcolor,
				 ImageObserver observer) {
	// TODO Auto-generated method stub
	return false;
    }

    @Override
	public void drawLine(int x1, int y1, int x2, int y2) {
	// TODO Auto-generated method stub

    }

    @Override
	public void drawOval(int x, int y, int width, int height) {
	// TODO Auto-generated method stub

    }

    @Override
	public void drawPolygon(int[] points, int[] points2, int points3) {
	// TODO Auto-generated method stub

    }

    @Override
	public void drawPolyline(int[] points, int[] points2, int points3) {
	// TODO Auto-generated method stub

    }

    @Override
	public void drawRoundRect(int x, int y, int width, int height,
				  int arcWidth, int arcHeight) {
	// TODO Auto-generated method stub

    }

    @Override
	public void fillArc(int x, int y, int width, int height, int startAngle,
			    int arcAngle) {
	// TODO Auto-generated method stub

    }

    @Override
	public void fillOval(int x, int y, int width, int height) {
	// TODO Auto-generated method stub

    }

    @Override
	public void fillPolygon(int[] points, int[] points2, int points3) {
	// TODO Auto-generated method stub

    }

    @Override
	public void fillRect(int x, int y, int width, int height) {
	// TODO Auto-generated method stub

    }

    @Override
	public void fillRoundRect(int x, int y, int width, int height,
				  int arcWidth, int arcHeight) {
	// TODO Auto-generated method stub

    }

    @Override
	public Shape getClip() {
	// TODO Auto-generated method stub
	return null;
    }

    @Override
	public Rectangle getClipBounds() {
	// TODO Auto-generated method stub
	return null;
    }

    @Override
	public Color getColor() {
	// TODO Auto-generated method stub
	return null;
    }

    @Override
	public Font getFont() {
	// TODO Auto-generated method stub
	return null;
    }

    @Override
	public FontMetrics getFontMetrics(Font f) {
	// TODO Auto-generated method stub
	return null;
    }

    @Override
	public void setClip(Shape clip) {
	// TODO Auto-generated method stub

    }

    @Override
	public void setClip(int x, int y, int width, int height) {
	// TODO Auto-generated method stub

    }

    @Override public void setColor(Color c) {
	c.getRGBComponents(currentColour);
    }

    @Override
	public void setFont(Font font) {
	// TODO Auto-generated method stub

    }

    @Override
	public void setPaintMode() {
	// TODO Auto-generated method stub

    }

    @Override
	public void setXORMode(Color c1) {
	// TODO Auto-generated method stub

    }

}
