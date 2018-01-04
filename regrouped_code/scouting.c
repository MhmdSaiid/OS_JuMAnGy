void limitObst(){
/*
this function is supposed to be used after detecting an obstacle during scouting.
it will turn around the obstacle then avoid it and put the robot towards the initial direction but after the obstacle
*/
	int end=0;	
	int distance = 5; //in cm
	rotate_car(90, 'R', SPEED_CIRCULAR)
	for(int side=1;	side<=5; side++){ //5 in order to do the missing part of the first side at the end

		while(!end){
			
			move(velocity,distance/velocity,0,'F');
			rotate_car(90, 'L', SPEED_CIRCULAR);
			if(!detect_obstacle()){
				end=1;
			}
			else{
				addToMap;
				rotate_car(90, 'R', SPEED_CIRCULAR);		
			}
			

		}
	}
	move(after the obstacle)
	rotate_car(90, 'L', SPEED_CIRCULAR);
	move(se realigner)
	rotate car(90, 'R', SPEED_CIRCULAR);
}
