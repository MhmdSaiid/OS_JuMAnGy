void limitObst(){
/*
this function is supposed to be used after detecting an obstacle during scouting.
it will turn around the obstacle then avoid it and put the robot towards the initial direction but after the obstacle
*/
	int end=0;	
	int distance = 5; //in cm
	float x_init = x_position;
	float y_init=y_position;
	rotate_car(90, 'R', SPEED_CIRCULAR);
	for(int side=1;	side<=5; side++){ //5 in order to do the missing part of the first side at the end

		while(!end){
			
			move(SPEED_LINEAR,distance/velocity,0,'F');
			rotate_car(90, 'L', SPEED_CIRCULAR);
			if(!detect_obstacle()){
				end=1;
				if(side==1){
					xDecal=abs(x_position-x_init);
					yDecal=abs(y_position-y_init);
					x_init=x_position;
					y_init = y_position;
				}
				if (side==2){
					xDecal2=abs(x_position-x_init);
					yDecal2=abs(y_position-y_init);				
				}
			}
			else{
				addToMap;
				rotate_car(90, 'R', SPEED_CIRCULAR);		
			}
			

		}
	}
	move(SPEED_LINEAR, max(xDecal,yDecal)/velocity, 0, 'F'); //longueur //go after the obstacle
	rotate_car(90, 'L', SPEED_CIRCULAR);
	move(SPEED_LINEAR, max(xDecal,yDecal)/velocity,0, 'F'); //largeur //realignment with initial position
	rotate car(90, 'R', SPEED_CIRCULAR);
}
