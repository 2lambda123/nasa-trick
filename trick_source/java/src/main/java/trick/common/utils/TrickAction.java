package trick.common.utils;

import java.awt.event.ActionEvent;
import java.lang.reflect.Method;
import java.util.Properties;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.ImageIcon;

import trick.common.TrickApplication;

public class TrickAction extends AbstractAction {
	private Properties props;
	private TrickApplication invoker;
	private Method man;
	
	
	public TrickAction(Properties p, TrickApplication target, Method m) {
		props = new Properties(p);
		invoker = target;
		man = m;
		applyProperties();
	}

	@Override
    public void actionPerformed(ActionEvent actionEvent) {
        // System.out.println(props.getProperty("text"));
        try {
	        man.invoke(invoker);
        } catch (Exception e) {
        	System.err.println(e.getMessage());
        }
    }
    
    @Override
	public boolean isEnabled() {
		return super.isEnabled();
    }
    
    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
    }
    
    @Override
    public void putValue(String key, Object value) {
    	if(value != null)  super.putValue(key, value);
    }
    
    @Override
    public String toString() {
    	return "";
    }
    
    private void applyProperties() {
    	String propValue;
    	
    	putValue(Action.NAME, props.getProperty(TEXT_PROPERTY));
    	putValue(Action.SHORT_DESCRIPTION, 
    			 props.getProperty(SHORT_DESCRIPTION_PROPERTY));
    	putValue(Action.LONG_DESCRIPTION,
    			 props.getProperty(LONG_DESCRIPTION_PROPERTY));
    	
    	if((propValue = props.getProperty(ICON_PROPERTY)) != null) {
    		putValue(Action.SMALL_ICON, getIcon(propValue));
    		putValue(Action.LARGE_ICON_KEY, getIcon(propValue));
    	} else {
    		if((propValue = props.getProperty(SMALL_ICON_PROPERTY)) != null) {
				putValue(Action.SMALL_ICON, getIcon(propValue));
			}
			
			if((propValue = props.getProperty(LARGE_ICON_PROPERTY)) != null) {
				putValue(Action.LARGE_ICON_KEY, getIcon(propValue));
			}
    	}
    }
    
    private ImageIcon getIcon(String fileName) {
		    String iconPath = invoker.getResourcePath(invoker.getClass());
    		int finalSlash = iconPath.lastIndexOf("/") + 1;
    		iconPath = iconPath.substring(0, finalSlash) + fileName;
    		return new ImageIcon(iconPath);
    }
    
    public static Properties extractProperties(Properties props, String name) {
		String head = name + ".Action.";
		String  textKey = head + TEXT_PROPERTY,
			    iconKey = head + ICON_PROPERTY,
			   sIconKey = head + SMALL_ICON_PROPERTY,
			   lIconKey = head + LARGE_ICON_PROPERTY, 
			   sDescKey = head + SHORT_DESCRIPTION_PROPERTY, 
			   lDescKey = head + LONG_DESCRIPTION_PROPERTY;
			   
		String textVal, sDescVal, lDescVal,
			   iconVal, sIconVal, lIconVal;
		
		Properties actionProp = new Properties();
		
		if((textVal = props.getProperty(textKey)) != null)
			actionProp.setProperty(TEXT_PROPERTY, textVal);
		
		if((sDescVal = props.getProperty(sDescKey)) != null) 
			actionProp.setProperty(SHORT_DESCRIPTION_PROPERTY, sDescVal);
		
		if((lDescVal = props.getProperty(lDescKey)) != null) 
			actionProp.setProperty(LONG_DESCRIPTION_PROPERTY, lDescVal);
		
		if((iconVal = props.getProperty(iconKey)) != null) 
			actionProp.setProperty(ICON_PROPERTY, iconVal);
		
		if((sIconVal = props.getProperty(sIconKey)) != null) 
			actionProp.setProperty(SMALL_ICON_PROPERTY, sIconVal);
		
		if((lIconVal = props.getProperty(lIconKey)) != null) 
			actionProp.setProperty(LARGE_ICON_PROPERTY, lIconVal);
		
		return actionProp;
    }
    
    
    private static final String TEXT_PROPERTY = "text";
    private static final String ICON_PROPERTY = "icon";
    private static final String SMALL_ICON_PROPERTY = "smallIcon";
    private static final String LARGE_ICON_PROPERTY = "largeIcon";
    private static final String SHORT_DESCRIPTION_PROPERTY = "shortDescription";
    private static final String LONG_DESCRIPTION_PROPERTY = "longDescription";
}
