package ch.psi.num.mountaingum.nexus.labelprovider;

import java.net.URL;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.graphics.Image;

import ch.psi.num.mountaingum.tree.TreeNode;
import ch.psi.num.mountaingum.ui.TreeViewer.TreeLabelProvider;

public class IconTreeLabelProvider extends TreeLabelProvider {

	Map<String, Image> classToIcon = new HashMap<String, Image>();

	Image defaultIcon;

	public IconTreeLabelProvider() {
		super();
		URL iconUrl = getClass().getResource("icons/unknown.gif");
		ImageDescriptor iconDes = ImageDescriptor.createFromURL(iconUrl);
		this.defaultIcon = iconDes.createImage(true);
	}

	@Override
	public Image getColumnImage(Object element, int columnIndex) {
		
		if (columnIndex != 0) return null;
		
		if (element instanceof TreeNode) {
			TreeNode node = (TreeNode) element;
			String type = node.getProperty("type");
			if (type != null) {
				if (classToIcon.containsKey(type))
					return classToIcon.get(type);
				else {
					URL iconUrl = getClass().getResource("icons/" + type + ".gif");
					ImageDescriptor iconDes = ImageDescriptor.createFromURL(iconUrl);
					Image icon = iconDes.createImage(false);
					if (icon == null) {
						System.out.println("dishing out default icon for " + type);
						icon = defaultIcon;
					}
					classToIcon.put(type, icon);
					return icon;
				}
			} else {
				System.out.println("IconTreeLabelProvider.getColumnImage() type null");
			}
		}

		System.out.println("dishing out default icon for " + element);

		Image columnImage = super.getColumnImage(element, columnIndex);
		return columnImage;
	}

}
