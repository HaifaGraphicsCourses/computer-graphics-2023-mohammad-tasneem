
=== Assignment1 part c ===

1) we define  a function called "Point adjustment" which imply an adjustment on the mesh model points to 
take place in the center of the window. the idea of the function:
    if (viewport_width_ > viewport_height_)
	{
		x = x * (viewport_height_ / constant1) + (viewport_width_ *constant2);
		y = y * (viewport_height_ / constant3) + (viewport_height_ * constant4);
	}
	else
	{
		x = x * (viewport_width_ / constant2) + (viewport_width_ * constant1);
		y = y * (viewport_width_ / constant4) + (viewport_height_ * constant3);
	}

more clear videos on the orthological projection :
	- orthoProjection_model
	- orthoProjection_addCamera
	- local_operations/features
	- orthological_pic

2) Camera class is inherited from model class, so that it has "translation_matrix" which been affected by the vector
	of the translation => (translate_x,translate_y,translate_z), once the user press on "L" letter the translate_x value been 
	incremented by negative value. and if we press "R" its incremented by positive value.

3) not supported yet in our project 
4) as shown in the video "Features" and image "show_local_world_axis"
	![enter image description here](show_local_world_axis.png)

5) shown in video "localoperations"


6) the idea is to go over the verticies and calculate the global minimum and the global maximum value so we get 8 points/vetritcies
    (taking in count the transformation of the model) for the bounding box, then we use drawline function to draw line between the verticies.
    shown in image "boundingBox" and in video "Features"

	![enter image description here](boundingBox.png)
	

7)for each Face we calculated the "normal" using the normula we learned then we scaled the normal up to see it at the on the screen, then we calculated the center of the face to Draw a line from there, this is the normal direction

shown in images:
	- facenormals
	- vertexnormals
	- video "Features"

![enter image description here](facenormals.png)
![enter image description here](vertexnormals.png)


8) we can compare with the videos :"orthoProjection_model" and "PerspectiveProjection"
    the diffrence is that the perspective is more realistic

9 - 11) not implemented


12) shown in image "twoCameras" 
![enter image description here](twoCameras.png)


