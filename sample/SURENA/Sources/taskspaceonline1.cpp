#include "Headers/taskspaceonline1.h"
TaskSpaceOnline1::TaskSpaceOnline1()
{


    time=0;
    _timeStep=0.00200000000000;
    globalTime=0;
    localTime=0;
    localTime1=0;
    localTiming= 0;
    SetParameters();
    CoeffArrayAnkle();
    CoeffArrayPelvis();

    localtimingInteger=0;
    bool _walkstate=true;
}

void TaskSpaceOnline1::SetParameters(){
    YOffsetOfAnkletrajectory=0.00;//for compensating the clearance of the hip roll in experiment

    toeOff=true;
    HipRollModification=true;


    HeelLandingAnglePitch=8*0;
    ToeOffAnglePitch=-6*0;
    Sc=0.3783;
    Rse=0.7364;
    Rd=0.5555;
    Rm=1.1095;
    Rzp=0.91;

    _lengthOfHip=0.10900;
    _lenghtOfAnkle=0.112000;
    _lengthOfShank=0.3600;
    _lengthOfThigh=0.3700;
    _heelLength=0.1;
    _toeLength=0.15;
    _pelvisLength=0.23;
    ReferencePelvisHeight=0.86;

    NStride=30;
    NStep=60;
    DesiredVelocity=0.20000;
    StepLength=0.170000;
    StepNumber=1;


    // localTime=0;

    /////////////////////////Note:the TC is inetgerrr!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    /// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\\\\\\\\\

    Tc=StepLength*3.6/DesiredVelocity;

    TStart=6;
    TEnd=6;
    TDs=.4*Tc; // Duration of double support phase
    T_beta=TDs/2;
    TSS=Tc-TDs; // Duration of single support phase
    TGait=TStart+NStride*2*Tc;

    XofAnkleMaximumHeight=(0.22/0.35)*StepLength; // Position of ankle in x direction when it reaches maximum heigth
    za_c=0.04;
    AnkleMaximumHeight=za_c; // maximum height of ankle
    za_st_m=AnkleMaximumHeight;
    //double motiontime;
    // motiontime=5;


    YpMax=1.1*Rm*0.5*_pelvisLength;
    Yd=1.1*Rd*YpMax;
    YEndMax=1.0*YpMax;
    YStMax=1.0*YpMax;
    Xs=Rse*Xe;
    xa_st_m=(0.22/0.35)*StepLength/2;
    za_st_m=(AnkleMaximumHeight);
    Xe=Sc*StepLength/(Rse+1);
    xa_end_m=2*NStride*StepLength+(0.22/0.35)*StepLength/2;
    za_end_m=(AnkleMaximumHeight);

    Xe=1*Sc*StepLength/(Rse+1);
    Xs=1.05*Rse*Xe;
    YpMax=1*Rm*0.5*_pelvisLength;
    Yd=1*Rd*YpMax;
    YStMax=1.0*YpMax;
    YEndMax=1.0*YpMax;
    cout<<" Xe="<<Xe<<" Xs="<< Xs <<" YpMax="<<YpMax<<" Yd="<<Yd<<" YStMax="<<YStMax<<" YEndMax="<<YEndMax<<endl;

    MotionTime=TStart+NStride*2*Tc+TDs+TEnd;

    TMinPelvisY=0.5*TDs; // The time that pelvis reaches its minimum distance in y direction
    TMaxAnkle=TDs+0.35*TSS;//0.53 % The time that ankle reaches its maximum distance in z direction
    TMaxPelvisY=TDs+0.5*TSS; // The time that pelvis reaches its maximum distance in y direction

    // last step: timing parameter of pelvis motion
    T_end_p_sx_rel=TDs+0.25*TEnd;
    T_end_p_sx=TGait+TDs+0.25*TEnd;
    T_end_p_sy=TGait+TDs+0.2*TEnd;
    T_end_p_dy=TGait+TDs+0.4*TEnd;
    T_end_p_ey=TGait+TDs+0.8*TEnd;
    // last step: timing parameter of ankle motion
    T_end_a_s=TGait+TDs;
    T_end_a_e=TGait+TDs+0.5*TEnd;
    T_end_a_d=TGait+TDs+0.45*(T_end_a_e-T_end_a_s);
    // first step: timing parameter of pelvis motion
    T_st_p_sy=0.2*TStart;
    T_st_p_dy=0.65*TStart;
    T_st_p_ey=0.8*TStart;
    T_st_p_sx=0.7*TStart;
    // first step: timing parameter of ankle motion
    T_st_a_s=0.5*TStart;
    T_st_a_d=T_st_a_s+0.45*(TStart-T_st_a_s);

}



void TaskSpaceOnline1::CoeffArrayPelvis(){

    //------------------Coefficient of cyclic motion in X direction--------------------
    MatrixXd ord(1,2);
    ord << 3,3;
    MatrixXd ttt(1,3);
    ttt <<0 ,TDs, Tc;
    MatrixXd con(3,3);
    con<<Xe, StepLength-Xs, StepLength+Xe,INFINITY, INFINITY ,INFINITY,INFINITY, INFINITY ,INFINITY;
    Cx_p_i.resize(2,6);
    Cx_p_i.fill(0);
    Cx_p_i.block(0,2,2,4)=CoefOffline.Coefficient1(ttt,ord,con,0.1).transpose();
    Cx_p.resize(1,12);
    Cx_p.fill(0);
    Cx_p.block(0,0,1,6)=Cx_p_i.row(0);
    Cx_p.block(0,6,1,6)=Cx_p_i.row(1);


    //------------------Coefficient of cyclic Pelvis motion in Y direction--------------------
    MatrixXd ordY(1,8);
    ordY << 3,3,4,4,3,3,4,4;
    MatrixXd tttY(1,9);
    tttY <<0,TMinPelvisY,TDs,TDs+TSS/2,Tc,Tc+TDs/2,Tc+TDs,Tc+TDs+TSS/2,2*Tc;
    MatrixXd conY(3,9);
    conY<<-1*Yd,0,Yd,YpMax,Yd,0,-1*Yd,-1*YpMax,-1*Yd,INFINITY, INFINITY,INFINITY, 0 ,INFINITY,INFINITY,INFINITY,0,INFINITY,INFINITY, INFINITY,INFINITY, INFINITY ,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY;
    Cy_p_i.resize(8,6);
    Cy_p_i.fill(0);
    Cy_p_i.block(0,1,8,5)=CoefOffline.Coefficient1(tttY,ordY,conY,0.1).transpose();
    Cy_p.resize(1,48);
    Cy_p.fill(0);


    Cy_p.block(0,0,1,6)=Cy_p_i.row(0);
    Cy_p.block(0,6,1,6)=Cy_p_i.row(1);
    Cy_p.block(0,12,1,6)=Cy_p_i.row(2);
    Cy_p.block(0,18,1,6)=Cy_p_i.row(3);
    Cy_p.block(0,24,1,6)=Cy_p_i.row(4);
    Cy_p.block(0,30,1,6)=Cy_p_i.row(5);
    Cy_p.block(0,36,1,6)=Cy_p_i.row(6);
    Cy_p.block(0,42,1,6)=Cy_p_i.row(7);



    //----------------Coefficient of Start of Pelvis motion in x direction---------------------
    MatrixXd Cx_st_pTime(1,2);
    Cx_st_pTime<<T_st_p_sx ,TStart;
    MatrixXd Cx_st_pPos(1,2);
    Cx_st_pPos<<0, Xe;
    MatrixXd Cx_st_pVel(1,2);
    Cx_st_pVel<<0 ,Cx_p(0,4);
    MatrixXd Cx_st_pAccel(1,2);
    Cx_st_pAccel<<0 ,2*Cx_p(0,3);
    Cx_st_p=CoefOffline.Coefficient(Cx_st_pTime,Cx_st_pPos,Cx_st_pVel,Cx_st_pAccel);

    // ----------------Coefficient of End of Pelvis motion in x direction----------------------

    MatrixXd Cx_end_pTime(1,2);
    Cx_end_pTime<<0,(T_end_p_sx_rel-TDs) ;
    MatrixXd Cx_end_pPos(1,2);
    Cx_end_pPos<<(NStep+1)*StepLength-Xs, (NStep+1)*StepLength;
    MatrixXd Cx_end_pVel(1,2);
    Cx_end_pVel<<5*Cx_p(0,0)*pow(TDs,4)+4*Cx_p(0,1)*pow(TDs,3)+3*Cx_p(0,2)*pow(TDs,2)+2*Cx_p(0,3)*pow(TDs,1)+Cx_p(0,4), 0;
    MatrixXd Cx_end_pAccel(1,2);
    Cx_end_pAccel<<5*4*Cx_p(0,0)*pow(TDs,4)+4*3*Cx_p(0,1)*pow(TDs,3)+3*2*Cx_p(0,2)*pow(TDs,2)+2*Cx_p(0,3), 0;//like the last moment of first part of trajectory of cycle
    Cx_end_p=CoefOffline.Coefficient(Cx_end_pTime,Cx_end_pPos,Cx_end_pVel,Cx_end_pAccel);


    //--------------Coefficient of Start of Pelvis motion in y direction----------------
    MatrixXd Cy_st_psTime(1,2);
    Cy_st_psTime<<T_st_p_sy ,T_st_p_dy;
    MatrixXd Cy_st_psPos(1,2);
    Cy_st_psPos<<0, -1*YStMax;
    MatrixXd Cy_st_psVel(1,2);
    Cy_st_psVel<<0 ,0;
    MatrixXd Cy_st_psAccel(1,2);
    Cy_st_psAccel<<0 ,0;
    Cy_st_pa=CoefOffline.Coefficient(Cy_st_psTime,Cy_st_psPos,Cy_st_psVel,Cy_st_psAccel);

    MatrixXd Cy_st_peTime(1,2);
    Cy_st_peTime<<T_st_p_ey, TStart;
    MatrixXd Cy_st_pePos(1,2);
    Cy_st_pePos<<-1*YStMax, -1*Yd;
    MatrixXd Cy_st_peVel(1,2);
    Cy_st_peVel<<0 ,5*Cy_p(0,0)*pow(0,4)+4*Cy_p(0,1)*pow(0,3)+3*Cy_p(0,2)*pow(0,2)+2*Cy_p(0,3)*pow(0,1)+Cy_p(0,4);
    MatrixXd Cy_st_peAccel(1,2);
    Cy_st_peAccel<<0 ,2*Cy_p(0,3);
    Cy_st_pb=CoefOffline.Coefficient(Cy_st_peTime,Cy_st_pePos,Cy_st_peVel,Cy_st_peAccel);

    // -------------------Coefficient of End of Pelvis motion in y direction--------------
    MatrixXd Cy_end_psTime(1,2);
    Cy_end_psTime<<TGait+TDs, T_end_p_sy;
    MatrixXd Cy_end_psPos(1,2);
    Cy_end_psPos<<Yd, YEndMax;
    MatrixXd Cy_end_psVel(1,2);
    Cy_end_psVel<<5*Cy_p_i(1,0)*pow(TDs,4)+4*Cy_p_i(1,1)*pow(TDs,3)+3*Cy_p_i(1,2)*pow(TDs,2)+2*Cy_p_i(1,3)*pow(TDs,1)+Cy_p_i(1,4),0;
    MatrixXd Cy_end_psAccel(1,2);
    Cy_end_psAccel<< 5*4*Cy_p_i(1,0)*pow(TDs,4)+4*3*Cy_p_i(1,1)*pow(TDs,3)+3*2*Cy_p_i(1,2)*pow(TDs,2)+2*Cy_p_i(1,3), 0;
    Cy_end_pa=CoefOffline.Coefficient(Cy_end_psTime,Cy_end_psPos,Cy_end_psVel,Cy_end_psAccel);

    MatrixXd Cy_end_peTime(1,2);
    Cy_end_peTime<<T_end_p_dy, T_end_p_ey;
    MatrixXd Cy_end_pePos(1,2);
    Cy_end_pePos<<YEndMax, 0;
    MatrixXd Cy_end_peVel(1,2);
    Cy_end_peVel<<0 ,0;
    MatrixXd Cy_end_peAccel(1,2);
    Cy_end_peAccel<<0 ,0;
    Cy_end_pb=CoefOffline.Coefficient(Cy_end_peTime,Cy_end_pePos,Cy_end_peVel,Cy_end_peAccel);

}


MatrixXd TaskSpaceOnline1::PelvisTrajectory(double time, int n, double localtiming,bool LastDSIndex){
    double N;
    double t;
    double xp;
    double yp;
    double zp;
    double dxp;
    double dyp;
    double dzp;
    double ddxp;
    double ddyp;
    double ddzp;
    //double localTime1=localTime;
    double Times;
    if (time<=TStart||time>TGait){
        N=0;
        t=time;
    }
    else if (time>TStart && time<TGait){
        N=floor((time-TStart)/(2*Tc));
        t=fmod((time-TStart),2*Tc)+TStart;
    }
    else if (time==TGait){
        N=NStride;
        t=0;
    }


    if (t<=T_st_p_sx){
        xp=0;
        dxp=0;
        ddxp=0;
        DoubleSupport=true;

    }
    else if (t>T_st_p_sx  && t<=TStart){
        MatrixXd output=GetAccVelPos(Cx_st_p,t,T_st_p_sx,5);
        xp=output(0,0);
        dxp=output(0,1);
        ddxp=output(0,2);
        DoubleSupport=true;
    }
    else if (t>TStart &&  t<=(TDs+TStart)){
        MatrixXd output=GetAccVelPos(Cx_p_i.row(0),t-TStart,0,5);
        xp=output(0,0);
        dxp=output(0,1);
        ddxp=output(0,2);
        DoubleSupport=true;
    }
    else if (t>(TDs+TStart) && t<=(Tc+TStart)){
        MatrixXd output=GetAccVelPos(Cx_p_i.row(1),t-TStart,0,5);
        xp=output(0,0);
        dxp=output(0,1);
        ddxp=output(0,2);
        DoubleSupport=false;
    }
    else if (t>(Tc+TStart) && t<=(Tc+TDs+TStart)){
        MatrixXd output=GetAccVelPos(Cx_p_i.row(0),t-Tc-TStart,0,5);
        xp=output(0,0)+StepLength;
        dxp=output(0,1);
        ddxp=output(0,2);
        DoubleSupport=true;
    }
    else if (t>(Tc+TDs+TStart) && t<=(2*Tc+TStart)){
        MatrixXd output=GetAccVelPos(Cx_p_i.row(1),t-Tc-TStart,0,5);
        xp=output(0,0)+StepLength;
        dxp=output(0,1);
        ddxp=output(0,2);
        DoubleSupport=false;
    }
    else if (t>TGait && t<(TGait+TDs)){
        MatrixXd output=GetAccVelPos(Cx_p_i.row(0),t-TGait,0,5);
        xp=output(0,0)+2*NStride*StepLength;
        dxp=output(0,1);
        ddxp=output(0,2);
        DoubleSupport=true;
    }
    else if (t>=(TGait+TDs) && t<T_end_p_sx){
        MatrixXd output=GetAccVelPos(Cx_end_p,t,(TGait+TDs),5);
        xp=output(0,0);
        dxp=output(0,1);
        ddxp=output(0,2);
        DoubleSupport=true;
    }
    else if (t>=T_end_p_sx  && t<=(TGait+TDs+TEnd)){
        xp=(2*NStride+1)*StepLength;
        dxp=0;
        ddxp=0;
        DoubleSupport=true;
    }

    if (TStart==0 && t==0){
        xp=Cx_p(4);
        dxp=0;
        ddxp=0;
    }

    xp=xp+2*StepLength*N;

    /// following was a try for online adapting of pelvis x motion,
    ///  however in last step we had some problems therefore we commented this and used previous offline version
    ///
//    if (time<TStart){
//        N=1;
//        t=time;
//        Times=time;
//    }
//    else if (time>=TStart && time<(TGait-0.001)){
//        N=floor((time-TStart-0.00001)/(Tc))+2;//
//        //
//        //-------------------------------------------------note------------------------------------------//
//        //above 0.00001 amount is decreased from time to handle the uncertainity of double numbers!!! we had a peak at the end of each step
//        //-------------------------------------------------note-------------------------------------------//
//        //cout<<N<<endl<<flush;
//        t=fmod((time-TStart),2*Tc)+TStart;
//        Times=fmod((time-TStart),Tc);
//    }
//    else if (time<=(TGait-0.001) && time>=(TGait+0.001) ) {
//        N=NStep+2;
//    }
//    else if (time>(TGait)){
//        N=NStep+2;
//        t=time;
//        Times=time-TGait;
//    }





////    if ((abs(localtiming-Times))>0.000001) {
////        double mil=(abs(localtiming-Times));
////        cout<<"time="<<Times<<endl<<endl;
////        cout<<"localtime="<<(localtiming)<<endl;
////        cout<<"Tstep="<<Tc<<endl;
////        //localtiming=0.002;
////                     cout<<"number of step="<<StepNumber<<endl;
////                     cout<<"N="<<N<<endl;
////        cout<<"different="<<mil<<endl<<endl;
////    }



//    //localtiming=Times;


//    if (N==1){
//        if (localtiming<=T_st_p_sx){
//            xp=0;
//            dxp=0;
//            ddxp=0;
//          //  DoubleSupport=true;
//        }
//        else {
//            MatrixXd output=GetAccVelPos(Cx_st_p,localtiming,T_st_p_sx,5);
//            xp=output(0,0);
//            dxp=output(0,1);
//            ddxp=output(0,2);
//          //  DoubleSupport=true;
//        }
//    }


//    if (N!=1 && N!=(NStep+2)) {

//        if (localtiming>0 &&  localtiming<(TDs)){
//            MatrixXd output=GetAccVelPos(Cx_p_i.row(0),localtiming,0,5);
//            xp=output(0,0);
//            dxp=output(0,1);
//            ddxp=output(0,2);
////cout<<xp<<endl<<flush;
//           // DoubleSupport=true;
//        }
//        else if (localtiming>=(TDs) && localtiming<=(Tc)){
//            MatrixXd output=GetAccVelPos(Cx_p_i.row(1),localtiming,0,5);
//            xp=output(0,0);
//            dxp=output(0,1);
//            ddxp=output(0,2);
//            if (time>65.5   && time<66.1) {
//                  cout<<xp<<" time= "<<time<<" timing= "<<localtiming<<" N= "<<N<<" Tds= "<<TDs<<endl<<flush;
//            }
//           // cout<<xp<<endl<<flush;
//           // DoubleSupport=false;
//        }
//    }


//    if ( N==(NStep+2) ) {

////        if (LastDSIndex==true) {
////            if (localtiming>=0 && localtiming<=(TDs+0.002)){
////                MatrixXd output=GetAccVelPos(Cx_p_i.row(0),localtiming,0,5);
////                xp=output(0,0);
////                dxp=output(0,1);
////                ddxp=output(0,2);

////            }
////        }
////        else {


////           if (localtiming>=0 && localtiming<=(T_end_p_sx_rel-TDs)){
////                MatrixXd output=GetAccVelPos(Cx_end_p,localtiming,0,5);
////                xp=output(0,0);
////                dxp=output(0,1);
////                ddxp=output(0,2);

////              //  DoubleSupport=true;
////            }
////            else if (localtiming>(T_end_p_sx_rel-TDs+0.002)  && localtiming<=(TEnd)){
////                xp=0;
////                dxp=0;
////                ddxp=0;
////              //  DoubleSupport=true;
////            }
////        }



//       if (t>TGait && t<(TGait+TDs)){
//            MatrixXd output=GetAccVelPos(Cx_p_i.row(0),t-TGait,0,5);
//            xp=output(0,0)+2*NStride*StepLength;
//            dxp=output(0,1);
//            ddxp=output(0,2);

//        }
//        else if (t>=(TGait+TDs) && t<T_end_p_sx){
//            MatrixXd output=GetAccVelPos(Cx_end_p,t,(TGait+TDs),5);
//            xp=output(0,0);
//            dxp=output(0,1);
//            ddxp=output(0,2);

//        }
//        else if (t>=T_end_p_sx  && t<=(TGait+TDs+TEnd)){
//            xp=(2*NStride+1)*StepLength;
//            dxp=0;
//            ddxp=0;

//        }


//    }

////    if (TStart==0 && t==0){
////        xp=Cx_p(4);
////        dxp=0;
////        ddxp=0;
////    }

//    if (time<=TStart) {
//        xp=xp+1*StepLength*(N-1);
//    }
//    else if (time<TGait) {
//        xp=xp+1*StepLength*(N-2);
//    }
//    else if (time<=(TGait-0.001)  &&  time>=(TGait+0.001)) {
//       xp=xp+1*StepLength*(N-2);
//    }
//    else if (time>=TGait && time<(TGait+TDs+0.002)){//sometimes 0.002 should be adde to second term
//        xp=xp+1*StepLength*(N-2);

//    }
//    else if (time>=(TGait+TDs) && time<T_end_p_sx){
//        xp=xp;

//    }
//    else if (time>T_end_p_sx) {
//        xp=1*StepLength*(N-1);
//    }

    //Y


    //    cout<<"time="<<localTime1<<endl;
    //    cout<<"timedsss="<<localtime<<endl;

    if(t<=T_st_p_sy){
        yp=0;
        dyp=0;
        ddyp=0;
    }
    else if (t>T_st_p_sy && t<=T_st_p_dy){
        MatrixXd output=GetAccVelPos(Cy_st_pa,t,T_st_p_sy,5);
        yp=output(0,0);
        dyp=output(0,1);
        ddyp=output(0,2);
        // polyval(Cy_st_a,t);
    }
    else if (t>T_st_p_dy && t<=T_st_p_ey){
        yp=-1*YStMax;
        dyp=0;
        ddyp=0;
    }
    else if (t>T_st_p_ey && t<=TStart){

        MatrixXd output=GetAccVelPos(Cy_st_pb,t,T_st_p_ey,5);
        yp=output(0,0);
        dyp=output(0,1);
        ddyp=output(0,2);
    }

    else if (t>TStart && t<=(TMinPelvisY+TStart)){
        MatrixXd output=GetAccVelPos(Cy_p_i.row(0),t-TStart,0,5);
        yp=output(0,0);
        dyp=output(0,1);
        ddyp=output(0,2);
        // yp=-polyval(Cy_p(6:10),t+Tc-TStart);
    }
    else if (t>(TMinPelvisY+TStart) && t<=(TDs+TStart)){
        MatrixXd output=GetAccVelPos(Cy_p_i.row(1),t-TStart,0,5);
        yp=output(0,0);
        dyp=output(0,1);
        ddyp=output(0,2);
        //yp=polyval(Cy_p(1:5),t-TStart);
    }
    else if (t>(TDs+TStart) && t<=(TMaxPelvisY+TStart)){
        MatrixXd output=GetAccVelPos(Cy_p_i.row(2),t-TStart,0,5);
        yp=output(0,0);
        dyp=output(0,1);
        ddyp=output(0,2);

        // yp=polyval(Cy_p(6:10),t-TStart);
    }
    else if (t>(TMaxPelvisY+TStart) && t<=(Tc+TStart)){
        MatrixXd output=GetAccVelPos(Cy_p_i.row(3),t-TStart,0,5);
        yp=output(0,0);
        dyp=output(0,1);
        ddyp=output(0,2);
        // yp=-polyval(Cy_p(1:5) ,t-Tc-TStart);
        //0,TMinPelvisY,TDs,Tmaxpelvis,Tc,Tc+Tminpelvis,Tc+TDs,Tc+Tmaxpelvis,2*Tc;
    }

    else if (t>(Tc+TStart) && t<=(Tc+TMinPelvisY+TStart)){
        MatrixXd output=GetAccVelPos(Cy_p_i.row(4),t-TStart,0,5);
        yp=output(0,0);
        dyp=output(0,1);
        ddyp=output(0,2);
        //yp=-polyval(Cy_p(6:10), t-Tc-TStart);
    }
    else if (t>(Tc+TMinPelvisY+TStart) && t<=(Tc+TDs+TStart)){
        MatrixXd output=GetAccVelPos(Cy_p_i.row(5),t-TStart,0,5);
        yp=output(0,0);
        dyp=output(0,1);
        ddyp=output(0,2);
        //yp=-polyval(Cy_p(6:10), t-Tc-TStart);
    }
    else if (t>(Tc+TDs+TStart) && t<=(Tc+TMaxPelvisY+TStart)){
        MatrixXd output=GetAccVelPos(Cy_p_i.row(6),t-TStart,0,5);
        yp=output(0,0);
        dyp=output(0,1);
        ddyp=output(0,2);
        //yp=-polyval(Cy_p(6:10), t-Tc-TStart);
    }
    else if (t>(Tc+TMaxPelvisY+TStart) && t<=(2*Tc+TStart)){
        MatrixXd output=GetAccVelPos(Cy_p_i.row(7),t-TStart,0,5);
        yp=output(0,0);
        dyp=output(0,1);
        ddyp=output(0,2);
        //yp=-polyval(Cy_p(6:10), t-Tc-TStart);
    }
    else if (t>TGait && t<=(TMinPelvisY+TGait)){
        MatrixXd output=GetAccVelPos(Cy_p_i.row(0),t-TGait,0,5);
        yp=output(0,0);
        dyp=output(0,1);
        ddyp=output(0,2);
        //yp=-polyval(Cy_p(6:10) , t+Tc-TGait);
    }
    else if (t>(TMinPelvisY+TGait) && t<=(TDs+TGait)){
        MatrixXd output=GetAccVelPos(Cy_p_i.row(1),t-TGait,0,5);
        yp=output(0,0);
        ddyp=output(0,2);
        dyp=output(0,1);
        //yp=polyval(Cy_p(1:5) , t-TGait);
    }
    else if (t>(TDs+TGait) && t<=T_end_p_sy){
        // yp=polyval(Cy_end_a,t);
        MatrixXd output=GetAccVelPos(Cy_end_pa,t,TGait+TDs,5);
        yp=output(0,0);
        dyp=output(0,1);
        ddyp=output(0,2);
    }
    else if (t>T_end_p_sy && t<=T_end_p_dy){
        yp=YEndMax;
        dyp=0;
        ddyp=0;
    }
    else if (t>T_end_p_dy && t<=T_end_p_ey){
        MatrixXd output=GetAccVelPos(Cy_end_pb,t,T_end_p_dy,5);
        yp=output(0,0);
        dyp=output(0,1);
        ddyp=output(0,2);

    }
    else if (t>T_end_p_ey && t<=(TGait+TDs+TEnd)){
        yp=0;
        dyp=0;
        ddyp=0;
    }
    if (TStart==0 && t==0){
        //  yp=-polyval(Cy_p(6:10),Tc);
    }

    zp=ReferencePelvisHeight;
    dzp=0;
    ddzp=0;



//    if(abs(globalVariableeee-currentLeftFootZ)>0.000000001){
//    cout<<"ohhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh= "<< abs(globalVariableeee-currentLeftFootZ) <<endl;
//    cout<<"ohhhhhhhhhhhhhhhhhhhhhhhhhhhh2222222222222222= "<< (currentLeftFootZ) <<endl;
//    cout<<"ohhhhhhhhhhhhhhhhhhhhhhhhhhhh1111111111111111= "<< (globalVariableeee) <<endl;
//    cout<<"ohhhhhhhhhhhhhhhhhhhhhhhhhhhh1111111111111111= "<< (localtiming) <<endl;

//    }


    MatrixXd pelvis(9,1);
    pelvis<<xp,yp,zp,dxp,dyp,dzp,ddxp,ddyp,ddzp;
    return pelvis;
}





void TaskSpaceOnline1::CoeffArrayAnkle(){


        T_s_st=.5*TStart;

        MatrixXd Cx_st_iTime_al(1,2);
        Cx_st_iTime_al<<T_s_st ,TStart;
        MatrixXd Cx_st_iPos_al(1,2);
        Cx_st_iPos_al<<0, StepLength;
        MatrixXd Cx_st_iVel_al(1,2);
        Cx_st_iVel_al<<0, 0;
        MatrixXd Cx_st_iAcc_al(1,2);
        Cx_st_iAcc_al<<0, 0;
        C_st_x_al=CoefOffline.Coefficient(Cx_st_iTime_al,Cx_st_iPos_al,Cx_st_iVel_al,Cx_st_iAcc_al);

        MatrixXd C_st_iTime(1,3);
        C_st_iTime<<T_s_st, TStart-T_s_st/2 ,TStart;
        MatrixXd C_st_iPos(1,3);
        C_st_iPos<<0, AnkleMaximumHeight,0;
        MatrixXd C_st_iVel(1,3);
        C_st_iVel<<0 ,INFINITY, 0;
        MatrixXd C_st_iAcc(1,3);
        C_st_iAcc<<0 ,INFINITY, 0;
        C_st_z_al=CoefOffline.Coefficient(C_st_iTime,C_st_iPos,C_st_iVel,C_st_iAcc);

        MatrixXd Cy_st_iTime(1,3);
        Cy_st_iTime<<T_s_st, TStart-T_s_st/2 ,TStart;
        MatrixXd Cy_st_iPos(1,3);
        Cy_st_iPos<<0.5*_pelvisLength,0.5*_pelvisLength+YOffsetOfAnkletrajectory, 0.5*_pelvisLength;
        MatrixXd Cy_st_iVel(1,3);
        Cy_st_iVel<<0 ,INFINITY, 0;
        MatrixXd Cy_st_iAcc(1,3);
        Cy_st_iAcc<<0 ,INFINITY, 0;
        C_st_y_al=CoefOffline.Coefficient(Cy_st_iTime,Cy_st_iPos,Cy_st_iVel,Cy_st_iAcc);

        MatrixXd C_cy_iTime_al(1,2);
        C_cy_iTime_al<<0, TSS;
        MatrixXd C_cy_iPos_al(1,2);
        C_cy_iPos_al<<0, 2*StepLength;
        MatrixXd C_cy_iVel_al(1,2);
        C_cy_iVel_al<<0, 0;
        MatrixXd C_cy_iAcc_al(1,2);
        C_cy_iAcc_al<<0, 0;
        C_cy_x_al=CoefOffline.Coefficient(C_cy_iTime_al,C_cy_iPos_al,C_cy_iVel_al,C_cy_iAcc_al);

        MatrixXd C_cy_iTime_ar(1,2);
        C_cy_iTime_ar<<0, TSS;
        MatrixXd C_cy_iPos_ar(1,2);
        C_cy_iPos_ar<<0, 2*StepLength;
        MatrixXd C_cy_iVel_ar(1,2);
        C_cy_iVel_ar<<0, 0;
        MatrixXd C_cy_iAcc_ar(1,2);
        C_cy_iAcc_ar<<0, 0;
        C_cy_x_ar=CoefOffline.Coefficient(C_cy_iTime_ar,C_cy_iPos_ar,C_cy_iVel_ar,C_cy_iAcc_ar);

        MatrixXd C_cy_iTime(1,3);
        C_cy_iTime<<0 ,TSS/2, TSS;
        MatrixXd C_cy_iPos(1,3);
        C_cy_iPos<<0,AnkleMaximumHeight, 0;
        MatrixXd C_cy_iVel(1,3);
        C_cy_iVel<<0 ,INFINITY, 0;
        MatrixXd C_cy_iAcc(1,3);
        C_cy_iAcc<<0 ,INFINITY, 0;
        C_cy_z_ar=CoefOffline.Coefficient(C_cy_iTime,C_cy_iPos,C_cy_iVel,C_cy_iAcc);

        MatrixXd C_end_z_iTime(1,3);
        C_end_z_iTime<<0 ,(T_end_a_e-T_end_a_s)/2 ,T_end_a_e-T_end_a_s;
        MatrixXd C_end_z_iPos(1,3);
        C_end_z_iPos<<0, AnkleMaximumHeight, 0;
        MatrixXd C_end_z_iVel(1,3);
        C_end_z_iVel<<0 ,INFINITY, 0;
        MatrixXd C_end_z_iAcc(1,3);
        C_end_z_iAcc<<0 ,INFINITY, 0;
        C_end_z_ar=CoefOffline.Coefficient(C_end_z_iTime,C_end_z_iPos,C_end_z_iVel,C_end_z_iAcc);

        MatrixXd C_end_y_iTime(1,3);
        C_end_y_iTime<<0 ,(T_end_a_e-T_end_a_s)/2 ,T_end_a_e-T_end_a_s;
        MatrixXd C_end_y_iPos(1,3);
        C_end_y_iPos<<-0.5*_pelvisLength,-0.5*_pelvisLength-YOffsetOfAnkletrajectory, -0.5*_pelvisLength;
        MatrixXd C_end_y_iVel(1,3);
        C_end_y_iVel<<0 ,INFINITY, 0;
        MatrixXd C_end_y_iAcc(1,3);
        C_end_y_iAcc<<0 ,INFINITY, 0;
        C_end_y_ar=CoefOffline.Coefficient(C_end_y_iTime,C_end_y_iPos,C_end_y_iVel,C_end_y_iAcc);

        MatrixXd C_end_iTime_ar(1,2);
        C_end_iTime_ar<<0, (T_end_a_e-T_end_a_s);
        MatrixXd C_end_iPos_ar(1,2);
        C_end_iPos_ar<<0, 1*StepLength;
        MatrixXd C_end_iVel_ar(1,2);
        C_end_iVel_ar<<0, 0;
        MatrixXd C_end_iAcc_ar(1,2);
        C_end_iAcc_ar<<0, 0;
        C_end_x_ar=CoefOffline.Coefficient(C_end_iTime_ar,C_end_iPos_ar,C_end_iVel_ar,C_end_iAcc_ar);


        MatrixXd Cy_cy_iTime(1,3);
        Cy_cy_iTime<<0 ,TSS/2, TSS;
        MatrixXd Cy_cy_iPos(1,3);
        Cy_cy_iPos<<-0.5*_pelvisLength,-0.5*_pelvisLength-YOffsetOfAnkletrajectory, -0.5*_pelvisLength;
        MatrixXd Cy_cy_iVel(1,3);
        Cy_cy_iVel<<0 ,INFINITY, 0;
        MatrixXd Cy_cy_iAcc(1,3);
        Cy_cy_iAcc<<0 ,INFINITY, 0;
        C_cy_y_ar=CoefOffline.Coefficient(Cy_cy_iTime,Cy_cy_iPos,Cy_cy_iVel,Cy_cy_iAcc);

        MatrixXd Cy_cy_lTime(1,3);
        Cy_cy_lTime<<0 ,TSS/2, TSS;
        MatrixXd Cy_cy_lPos(1,3);
        Cy_cy_lPos<<0.5*_pelvisLength,0.5*_pelvisLength+YOffsetOfAnkletrajectory, 0.5*_pelvisLength;
        MatrixXd Cy_cy_lVel(1,3);
        Cy_cy_lVel<<0 ,INFINITY, 0;
        MatrixXd Cy_cy_lAcc(1,3);
        Cy_cy_lAcc<<0 ,INFINITY, 0;
        C_cy_y_al=CoefOffline.Coefficient(Cy_cy_lTime,Cy_cy_lPos,Cy_cy_lVel,Cy_cy_lAcc);




}


MatrixXd TaskSpaceOnline1::AnkleTrajectory(double time,int n, double localtiming,bool RFT_state,bool LFT_state,bool LastDSIndex){

    double y_ar=-0.5*_pelvisLength;
    double y_al=0.5*_pelvisLength;
    double alpha_ar=0;
    double alpha_al=0;
    double beta_al=0;
    double beta_ar=0;
    double gama_ar=0;
    double gama_al=0;
    double x_ar;
    double z_ar;
    double x_al;
    double z_al;
    double pitch_al=0;
    double pitch_ar=0;
   // int Nn;



    if (n==1){//left foot moves in first step
        x_ar=0;
        z_ar=_lenghtOfAnkle;
        x_al=0;
        z_al=_lenghtOfAnkle;

        if (localtiming>=T_s_st){
            MatrixXd output=GetAccVelPos(C_st_x_al.row(0),localtiming,T_s_st,5);
            x_al=output(0,0);

            if (localtiming<=TStart-T_s_st/2){
                MatrixXd output=GetAccVelPos(C_st_z_al.row(0),localtiming,T_s_st,5);
                z_al=_lenghtOfAnkle+output(0,0);
                //LeftSupport=false;
            }
            else{
                MatrixXd output=GetAccVelPos(C_st_z_al.row(1),localtiming,TStart-T_s_st/2,5);
                z_al=_lenghtOfAnkle+output(0,0);
               // LeftSupport=false;
            }
        }
    }

     if (n!=1 && n!=(NStep+2)){//cyclic walking

n=n-1;
//cout<<"localtime"<<localtiming<<endl;
        int tempIndex=fmod(n,2);// shows which foot is swing foot (in cyclic mode left foots is swinging in the even steps(N))

        if (localtiming<TDs){// double support of cyclic walking
            z_ar=currentRightFootZ;
            z_al=currentLeftFootZ;

            if (tempIndex!=0) {
                x_ar=currentRightFootX1;
                x_al=currentLeftFootX1;//;

                if (abs((n)*StepLength-currentLeftFootX1)>0.0001) {
                    cout<<(n)*StepLength<<endl;
                    cout<<"tt="<<localtiming<<endl;
                    cout<<"warning!!! current left= "<<currentLeftFootX1<<endl;
                }
                if (abs((n-1)*StepLength-currentRightFootX1)>0.0001) {
                    cout<<(n-1)*StepLength<<endl;
                    cout<<"warning!!! current right= "<<currentRightFootX1<<endl;
                }


               //  LeftSupport=true;
            }
            else {
                x_ar=currentRightFootX1;
                x_al=currentLeftFootX1;
               // LeftSupport=true;
            }
        }

        else if (localtiming<Tc){//single support of cyclic walking

                MatrixXd output1=GetAccVelPos(C_cy_x_ar,localtiming-TDs,0,5);
                x_ar=(tempIndex!=0)*((currentRightFootX1+output1(0,0)))+(tempIndex==0)*currentRightFootX1;
                x_al=(tempIndex!=0)*currentLeftFootX1+(tempIndex==0)*((currentLeftFootX1+output1(0,0)));

                if (tempIndex!=0) {
               //     LeftSupport=true;
                }
                else {
                //    LeftSupport=false;
                }

                if (localtiming<TDs+TSS/2){
                    MatrixXd output2=GetAccVelPos(C_cy_z_ar.row(0),localtiming-TDs,0,5);
                    z_al=(tempIndex!=0)*currentLeftFootZ+(tempIndex==0)*(currentLeftFootZ+output2(0,0));
                    z_ar=(tempIndex!=0)*(currentRightFootZ+output2(0,0))+(tempIndex==0)*currentRightFootZ;
                }
                else{
                    MatrixXd output2=GetAccVelPos(C_cy_z_ar.row(1),localtiming-TDs,TSS/2,5);
                    z_al=(tempIndex!=0)*currentLeftFootZ+(tempIndex==0)*((currentLeftFootZ+output2(0,0)));
                    z_ar=(tempIndex!=0)*((currentRightFootZ+output2(0,0)))+(tempIndex==0)*currentRightFootZ;
                }
        }

    }

    if(n==NStep+2){//end step of walk right foot moves
        if (LastDSIndex==true) {
            n=n-1;


         //   cout<<"ohhhhh= "<< (currentLeftFootX1) <<endl;


            int tempIndex=fmod(n,2);// shows which foot is swing foot (in cyclic mode left foots is swinging in the even steps(N))

            if (localtiming<=(TDs+0.001)){// double support of cyclic walking
                z_ar=currentRightFootZ;
                z_al=currentLeftFootZ;
//                cout<<"warning!!! current left= "<<currentLeftFootX1<<endl;
//                cout<<"warning!!! current Right= "<<currentRightFootX1<<endl;
//                if (tempIndex!=0) {
//                    x_ar=currentRightFootX1;
//                    x_al=currentLeftFootX1;//;

//                }
//                else {
                    x_ar=currentRightFootX1;
                    x_al=currentLeftFootX1;
                   // LeftSupport=true;
//                }
                    if (time>37.19  && time<37.25) {
                                  cout<<x_ar<<" timing= "<<time<<endl<<flush;
                }

            }

        }
        else {
            if (localtiming<=(T_end_a_e-T_end_a_s)){

                MatrixXd output1=GetAccVelPos(C_end_x_ar.row(0),localtiming,0,5);

                x_ar=currentRightFootX1+output1(0,0);

                x_al=currentLeftFootX1;//*/(2*NStride+1)*StepLength;/*
                z_al=currentLeftFootZ;

//                 cout<<"warning!!! current left= "<<currentLeftFootX1<<endl;
//                 cout<<"warning!!! current Right= "<<currentRightFootX1<<endl;
                if (localtiming<=(T_end_a_e-T_end_a_s)/2){
                    MatrixXd output2=GetAccVelPos(C_end_z_ar.row(0),localtiming,0,5);
                    z_ar=currentRightFootZ+output2(0,0);//currentRightFootZ+output2(0,0);
                }

                else if (localtiming>=(T_end_a_e-T_end_a_s)/2){
                    MatrixXd output2=GetAccVelPos(C_end_z_ar.row(1),localtiming, (T_end_a_e-T_end_a_s)/2,5);
                    z_ar=currentRightFootZ+output2(0,0);

                }


                if (time>37.2  && time<37.25) {
                              cout<<x_ar<<" timing= "<<time<<endl<<flush;
            }


            }
            else{

            //     cout<<"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj= "<<currentLeftFootX1<<endl;

                x_ar=currentRightFootX1;
                z_ar=currentRightFootZ;

                x_al=currentLeftFootX1;
                z_al=currentLeftFootZ;
            }
        }
       // double tt=time-(MotionTime-TEnd);

    }
    //cout<<currentRightFootX1<<endl;
  // cout<<localtiming<<endl;
//globalVariableeee=z_al;
    MatrixXd footpos(8,1);
    footpos<<x_al,y_al,z_al,pitch_al,x_ar,y_ar,z_ar,pitch_ar;
    return footpos;

}



MatrixXd TaskSpaceOnline1::GetAccVelPos(MatrixXd Coef,double time,double ti,int PolynomialOrder)
{
    int PolyNomialDegree=PolynomialOrder;
    MatrixXd T(PolyNomialDegree+1,1);
    T.fill(0);
    MatrixXd Diag(PolyNomialDegree+1,PolyNomialDegree+1);
    Diag.fill(0);
    for (int var = 0; var < PolyNomialDegree+1; var++) {
        T(var,0)=pow((time-ti),PolyNomialDegree-var);
        if (var!=0) {
            Diag.diagonal(1)(var-1,0)=PolyNomialDegree-var+1;

        }
    }

    MatrixXd x=Coef*T;
    double X=x(0,0);

    MatrixXd v=Coef*Diag*T;
    double V=v(0,0);

    MatrixXd a=Coef*Diag*Diag*T;
    double A=a(0,0);

    MatrixXd Output(1,3);
    Output<<X,V,A;
    return Output;
}
