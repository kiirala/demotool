import java.awt.*;
import java.awt.geom.*;

import org.apache.batik.bridge.BridgeExtension;
import org.apache.batik.bridge.ExternalResourceSecurity;
import org.apache.batik.bridge.ScriptSecurity;
import org.apache.batik.bridge.UserAgent;
import org.apache.batik.gvt.event.EventDispatcher;
import org.apache.batik.gvt.text.Mark;
import org.apache.batik.util.ParsedURL;
import org.apache.batik.util.XMLResourceDescriptor;
import org.w3c.dom.Element;
import org.w3c.dom.svg.SVGAElement;

/**
 * Toimii äärimmäisen yksinkertaisena user agentina Batikille. Ei tarjoa
 * mitään ohjelmassa oleellista toiminnallisuutta, mutta SVG-muuntimelle
 * oleellinen osa Batikia vaatii jonkin tällaisen luokan.
 *
 * @author Niko Kiirala
 */
public class SimpleUserAgent implements UserAgent {

	@Override
	public void checkLoadExternalResource(ParsedURL resourceURL,
			ParsedURL docURL) throws SecurityException {
		// TODO Auto-generated method stub

	}

	@Override
	public void checkLoadScript(String scriptType, ParsedURL scriptURL,
			ParsedURL docURL) throws SecurityException {
		// TODO Auto-generated method stub

	}

	@Override
	public void deselectAll() {
		// TODO Auto-generated method stub

	}

	@Override
	public void displayError(Exception ex) {
		System.out.println(ex.toString());
	}

	@Override
	public void displayMessage(String message) {
		System.out.println(message);
	}

	@Override
	public String getAlternateStyleSheet() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public float getBolderFontWeight(float f) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public Point getClientAreaLocationOnScreen() {
	    return new Point(0, 0);
	}

	@Override
	public String getDefaultFontFamily() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public EventDispatcher getEventDispatcher() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public ExternalResourceSecurity getExternalResourceSecurity(
			ParsedURL resourceURL, ParsedURL docURL) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getLanguages() {
		return "en";
	}

	@Override
	public float getLighterFontWeight(float f) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public String getMedia() {
		return "print";
	}

	@Override
	public float getMediumFontSize() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public float getPixelToMM() {
		return getPixelUnitToMillimeter();
	}

	@Override
	public float getPixelUnitToMillimeter() {
		return (float) 0.28222222;
	}

	@Override
	public ScriptSecurity getScriptSecurity(String scriptType,
			ParsedURL scriptURL, ParsedURL docURL) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public AffineTransform getTransform() {
		return new AffineTransform();
	}

	@Override
	public String getUserStyleSheetURI() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Dimension2D getViewportSize() {
	    return new Dimension(1280, 720);
	}

	@Override
	public String getXMLParserClassName() {
		return XMLResourceDescriptor.getXMLParserClassName();
	}

	@Override
	public void handleElement(Element elt, Object data) {
		// TODO Auto-generated method stub

	}

	@Override
	public boolean hasFeature(String s) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isXMLParserValidating() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void registerExtension(BridgeExtension ext) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setSVGCursor(Cursor cursor) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setTextSelection(Mark start, Mark end) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setTransform(AffineTransform at) {
		// TODO Auto-generated method stub

	}

	@Override
	public void showAlert(String message) {
		// TODO Auto-generated method stub

	}

	@Override
	public boolean showConfirm(String message) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public String showPrompt(String message) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String showPrompt(String message, String defaultValue) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public boolean supportExtension(String s) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void openLink(SVGAElement elt) {
		// TODO Auto-generated method stub
		
	}
}
