﻿#include "Headers/Robot.h"
#include "Headers/mainwindow.h"
////note that ID is one unit more than Links index
Robot::Robot()
{
    QString address = QDir::currentPath();
    //QStringList addressList = address.split("/bin");
    //address = addressList.value(0);
    address += "/test.txt";
    // cout << address.toStdString() <<endl<<flush;

    if (!FileExists(address))
    {
        qWarning()<<"Invalid Robot data Path:"<<address;
        return;
    }
    QFile file(address);
    file.open(QFile::ReadWrite);
    content = file.readAll();


    Links= CreateRobotLinks( content );
    ForwardKinematicPrimary(1);
    MatrixXi rightRoute= GetRightLegRoute();
    MatrixXi leftRoute= GetLeftLegRoute();
    // MatrixXi _LegRoute= FindRoutfromRoot2Destination("LArm_WristP_J7");
    MatrixXd rightLegAngle(1,6);
    MatrixXd leftLegAngle(1,6);
    //rightLegAngle<<0 ,0 ,-25 ,50, -25, 0;
    rightLegAngle<<0 ,0 ,-1.5,3,  -1.5, 0;

    leftLegAngle<<0 ,0 ,-1.5 ,3,  -1.5, 0;
    SetJointAngle(rightLegAngle*(M_PI/180),rightRoute);
    SetJointAngle(leftLegAngle*(M_PI/180),leftRoute);



    //  MatrixXd q_m;
    //MatrixXd x_m;

    VectorXd x;
    // x.setLinSpaced(81,0,0.2);
    // int Nstep = x.rows();
    // q_m = MatrixXd::Zero(Nstep,6);
    //x_m =  MatrixXd::Zero(Nstep,1);
    // LinkM Rfoot=Links[MapingName2ID.value("RLEG_J6")-1];
    //    double errorm;
    //    for (int var = 0; var < Nstep; var++) {
    //        Rfoot.PositionInWorldCoordinate(0)=x(var);
    //        errorm=IKLevenbergMarquardt(Rfoot,"RLEG_J6");
    //qDebug()<<errorm;
    //    }


    file.close();

}


QList<LinkM> Robot::GetLinks()
{
    return Links;
}

MatrixXi Robot::Getroute()
{
    return _route;
}

MatrixXi Robot::GetLeftLegRoute()
{
    MatrixXi _leftLegRoute= FindRoutfromRoot2Destination("LLeg_AnkleR_J6");
    return _leftLegRoute;
}

void Robot::SetLeftLegRoute(MatrixXd leftLegRoute)
{
    _leftLegRoute=leftLegRoute;
}

MatrixXi Robot::GetRightLegRoute()
{
    MatrixXi _rightLegRoute= FindRoutfromRoot2Destination("RLeg_AnkleR_J6");
    return _rightLegRoute;
}

void Robot::SetRightLegRoute(MatrixXd rightLegRoute)
{
    _rightLegRoute=rightLegRoute;
}

QList <QByteArray> Robot::GetContentOfRobot(QString name,QByteArray content)
{
    QList <QByteArray> result;
    int index=0;
    while (index<content.length()) {

        index=content.indexOf(name,index);
        if (index<0)
        {
            break;
        }

        QByteArray temp=content.mid(index,content.length());
        QByteArray line=temp.split('\n')[0];
        QList <QByteArray> values=line.split('=');
        if (values.length()>1){
            result.append(values[1]);
        }
        else
        {
            qDebug()<<"warning:expected '=' after value"<<name;
        }
        index++;
    }
    return result;
}


bool Robot::FileExists(QString path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}



MatrixXd Robot::ExtractionOfMatrix(QByteArray data)
{
    MatrixXd mat;
    QByteArray insideBrackets=data.split(']')[0];
    insideBrackets= insideBrackets.split('[')[1];
    QList <QByteArray> rows=insideBrackets.split(';');
    QList <QByteArray> columns=rows[0].split(',');

    mat.resize(rows.length(),columns.length());

    //////initial mat values
    for (int i = 0; i < rows.length(); i++) {
        QList <QByteArray> currentCols=rows[i].split(',');
        for (int j = 0; j < currentCols.length(); j++) {
            mat(i,j) = currentCols[j].toDouble();

        }
    }


    return mat;
}



QList<LinkM> Robot::CreateRobotLinks(QByteArray content )
{
    QList<QByteArray> NameOfLinks= GetContentOfRobot("Name",content);
    QList<QByteArray> IDofLink= GetContentOfRobot("JointID",content);
    QList<QByteArray> IDofSister= GetContentOfRobot("SisterID",content);
    QList<QByteArray> IDofChild= GetContentOfRobot("ChildID",content);
    QList<QByteArray> IDofMother= GetContentOfRobot("MotherID",content);
    QList<QByteArray> AngleOfJoint= GetContentOfRobot("JointAngle",content);
    QList<QByteArray> LocalAxisVectorOfJoint= GetContentOfRobot("JointAxisVectorLocal",content);
    QList<QByteArray> PositionRelative2ParentOfJoint= GetContentOfRobot("JointPositionRelative2Parent",content);
    QList<QByteArray> CenterOfMassLocal=GetContentOfRobot("CenterOfMassLocal",content);
    QList<QByteArray> AttitudeInWorldCoordinate=GetContentOfRobot("AttitudeInWorldCoordinate",content);
    QList<QByteArray>  MomentumOfInertia=GetContentOfRobot("MomentumOfInertia",content);
    QList<QByteArray> Mass=GetContentOfRobot("Mass",content);


    for (int index = 0; index < NameOfLinks.count(); index++) {

        QString linkName=QString::fromLatin1(NameOfLinks[index].data());
        int iDofSister=QString::fromLatin1(IDofSister[index].data()).toInt();
        int iDofChild=QString::fromLatin1(IDofChild[index].data()).toInt();
        int iDofLink=QString::fromLatin1(IDofLink[index].data()).toInt();
        double angleOfJoint=QString::fromLatin1(AngleOfJoint[index].data()).toDouble();
        double mass=QString::fromLatin1(Mass[index].data()).toDouble();
        int iDofMother=QString::fromLatin1(IDofMother[index].data()).toInt();
        MatrixXd centerOfMassLocal= ExtractionOfMatrix(CenterOfMassLocal[index]);
        MatrixXd attitudeInWorldCoordinate= ExtractionOfMatrix(AttitudeInWorldCoordinate[index]);
        MatrixXd momentumOfInertia= ExtractionOfMatrix(MomentumOfInertia[index]);
        MatrixXd localAxisVectorOfJoint= ExtractionOfMatrix(LocalAxisVectorOfJoint[index]);
        MatrixXd positionRelative2ParentOfJoint= ExtractionOfMatrix(PositionRelative2ParentOfJoint[index]);
        LinkM templlink(linkName,iDofLink,iDofSister,iDofChild,iDofMother,angleOfJoint,localAxisVectorOfJoint,positionRelative2ParentOfJoint,centerOfMassLocal,attitudeInWorldCoordinate,momentumOfInertia,mass);
        Links.append(templlink);
        MapingName2ID.insert(Links[index].GetName(),Links[index].GetID());
        MapingID2Name.insert(Links[index].GetID(),Links[index].GetName());
        //  qDebug()<<Links[index].GetName();
    }

    return Links;
}



MatrixXd Robot::Rodrigues(MatrixXd omega,double angle)
{
    MatrixXd rotation;
    double normOfOmega=omega.norm();

    if (normOfOmega<std::numeric_limits<double>::epsilon()) {

        rotation= MatrixXd::Identity(3,3);
    } else {
        MatrixXd NormalizedAxisRotation=omega/normOfOmega;;
        double AmountOfRotation=normOfOmega*angle;
        Matrix3d SkewSymMatrixOfVector;
        SkewSymMatrixOfVector<<0,
                -NormalizedAxisRotation(2),
                NormalizedAxisRotation(1),
                NormalizedAxisRotation(2),
                0,
                -NormalizedAxisRotation(0),
                -NormalizedAxisRotation(1),
                NormalizedAxisRotation(0),
                0;
        rotation=MatrixXd::Identity(3,3)+SkewSymMatrixOfVector*qSin(AmountOfRotation)+(SkewSymMatrixOfVector*SkewSymMatrixOfVector)*(1-qCos(AmountOfRotation));
    }
    return rotation;
}



void Robot::ForwardKinematic(int input)
{
    if (input==0) {
        return;
    }

    if (input!=1) {
        MatrixXd positionInWorldCoordinate=Links[Links[input-1].GetMotherID()-1].AttitudeInWorldCoordinate*Links[input-1].GetJointPositionRelative2Parent()+Links[Links[input-1].GetMotherID()-1].PositionInWorldCoordinate;
        MatrixXd attitudeInWorldCoordinate=Links[Links[input-1].GetMotherID()-1].AttitudeInWorldCoordinate*Rodrigues(Links[input-1].GetJointAxisVectorLocal(),Links[input-1].JointAngle);

        Links[input-1].AttitudeInWorldCoordinate=attitudeInWorldCoordinate;
        Links[input-1].PositionInWorldCoordinate=positionInWorldCoordinate;

    }
    ForwardKinematic( Links[input-1].GetChildID());
    ForwardKinematic( Links[input-1].GetSisterID());

}

void Robot::ForwardKinematicPrimary(int input)
{
    if (input==0) {
        return;
    }

    if (input!=1) {
        MatrixXd positionInWorldCoordinate=Links[Links[input-1].GetMotherID()-1].AttitudeInWorldCoordinate*Links[input-1].GetJointPositionRelative2Parent()+Links[Links[input-1].GetMotherID()-1].PositionInWorldCoordinate;
        MatrixXd attitudeInWorldCoordinate=Links[Links[input-1].GetMotherID()-1].AttitudeInWorldCoordinate*Rodrigues(Links[input-1].GetJointAxisVectorLocal(),Links[input-1].JointAngle);

        Links[input-1].AttitudeInWorldCoordinate=attitudeInWorldCoordinate;
        Links[input-1].PositionInWorldCoordinate=positionInWorldCoordinate;

    }  else {
        QList <QByteArray> PositionInWorldCoordinate= GetContentOfRobot("PositionInWorldCoordinate",content);
        MatrixXd positionInWorldCoordinate= ExtractionOfMatrix( PositionInWorldCoordinate[input-1]);
        QList <QByteArray> AttitudeInWorldCoordinate= GetContentOfRobot("AttitudeInWorldCoordinate",content);
        MatrixXd attitudeInWorldCoordinate= ExtractionOfMatrix( AttitudeInWorldCoordinate[input-1]);
        Links[input-1].AttitudeInWorldCoordinate=attitudeInWorldCoordinate;
        Links[input-1].PositionInWorldCoordinate=positionInWorldCoordinate;
    }
    ForwardKinematic( Links[input-1].GetChildID());
    ForwardKinematic( Links[input-1].GetSisterID());

}

void Robot::ForwardKinematicCoM(int input)
{
    if (input==0) {
        return;
    }

    if (input!=1) {
        MatrixXd centerOfMassInWorldCoordinate=Links[Links[input-1].GetID()-1].PositionInWorldCoordinate+Links[Links[input-1].GetID()-1].AttitudeInWorldCoordinate*Links[input-1].GetCenterOfMassLocal();
        Links[input-1].CenterOfMassInWorldCoordinate=centerOfMassInWorldCoordinate;

    }
    ForwardKinematicCoM( Links[input-1].GetChildID());
    ForwardKinematicCoM( Links[input-1].GetSisterID());

}

void Robot::CalcMC(int input){




}

void Robot::SetJointAngle(MatrixXd jointAngle, MatrixXi Route){

    for (int var = 0; var < Route.cols(); var++) {
        int jointNumber;
        jointNumber=Route(0,var);
        Links[jointNumber-1].JointAngle=jointAngle(0,var);
    }
    ForwardKinematic(1);
    ForwardKinematicCoM(1);
}


void Robot::MoveJoints(MatrixXi route, MatrixXd deltaJointAngle)
{int index;
    for (int var = 0; var < route.cols(); var++) {

        index=route(var)-1;
        Links[index].JointAngle=Links[index].JointAngle+deltaJointAngle(var);

    }

}



MatrixXi Robot::FindRoutfromRoot2Destination(QString Destination)
{
    int to= MapingName2ID.value(Destination);
    MatrixXi toMatrix(1,1);
    toMatrix.fill(0);
    toMatrix<<to;
    int mother=Links[to-1].GetMotherID();
    if (mother==1)
        return toMatrix;

    else
    {
        MatrixXi _route=FindRoutfromRoot2Destination(MapingID2Name.value(mother));
        _route.conservativeResize(NoChange,_route.cols()+1);
        _route.block(0,_route.cols()-1,1,1) = toMatrix;
        return _route;

    }

}


MatrixXi Robot::FindRoutfromBeginning2Destination(QString Beginning,QString Destination)
{
    int from= MapingName2ID.value(Beginning);
    int to= MapingName2ID.value(Destination);
    MatrixXi toMatrix(1,1);
    toMatrix.fill(0);
    toMatrix<<to;
    int mother=Links[to-1].GetMotherID();
    if (mother==from)
        return toMatrix;

    else
    {
        MatrixXi _route=FindRoutfromBeginning2Destination(Beginning,MapingID2Name.value(mother));
        _route.conservativeResize(NoChange,_route.cols()+1);
        _route.block(0,_route.cols()-1,1,1) = toMatrix;
        return _route;

    }

}


MatrixXi Robot::FindRoutfromDestination2Beginning(QString Beginning,QString Destination)
{
    int from= MapingName2ID.value(Beginning);
    int to= MapingName2ID.value(Destination);
    MatrixXi toMatrix(1,1);
    toMatrix.fill(0);
    toMatrix<<to;
    int mother=Links[to-1].GetMotherID();
    if (mother==from)
        return toMatrix;

    else
    {
        MatrixXi _route=FindRoutfromBeginning2Destination(Beginning,MapingID2Name.value(mother));
        _route.conservativeResize(NoChange,_route.cols()+1);
        _route.block(0,_route.cols()-1,1,1) = toMatrix;
        return _route;

    }

}


int Robot::Sign(double v) {
    return (v < 0) ? -1 : ((v > 0) ? 1 : 0);
}

MatrixXd Robot::RPitch(double theta){
    MatrixXd Ry(3,3);
    double c=cos(theta);
    double s=sin(theta);
    Ry<<c,0,s,0,1,0,-1*s,0,c;
    return Ry;
}

MatrixXd Robot::RRoll(double phi){
    MatrixXd R(3,3);
    double c=cos(phi);
    double s=sin(phi);
    R<<1,0,0,0,c,-1*s,0,s,c;
    return R;
}


Vector3d Robot::Rot2omega(Matrix3d rotation)
{
    Vector3d omega;
    Vector3d el;
    Vector3d temp;
    omega.fill(0);
    el.fill(0);
    temp.fill(0);
    //    omega.resize(3,1);
    //    el.resize(3,1);
    //    temp.resize(3,1);
    el<< rotation(2,1)-rotation(1,2),rotation(0,2)-rotation(2,0),rotation(1,0)-rotation(0,1);
    double norm_el= el.norm();
    if (norm_el>std::numeric_limits<double>::epsilon()) {
        omega<<(qAtan2(norm_el,rotation.trace()-1)/norm_el *el);
    }

    else if (rotation(0,0)>0 && rotation(1,1)>0 && rotation(2,2)>0) {
        omega<<0,0,0;
    }

    else {
        temp<<rotation(0,0)+1,rotation(1,1)+1,rotation(2,2)+1;
        omega<<(M_PI/2)*temp;
    }


    return omega;


}


MatrixXd Robot::CalcTaskSpaceError(LinkM Target, QString current)
{
    Vector3d PositionError;
    PositionError.fill(0);
    Matrix3d RotationError;
    RotationError.fill(0);
    Vector3d AttitudeError;
    AttitudeError.fill(0);
    MatrixXd TaskSpaceError;
    TaskSpaceError.resize(6,1);
    TaskSpaceError.fill(0);
    PositionError=Target.PositionInWorldCoordinate-Links[MapingName2ID.value(current)-1].PositionInWorldCoordinate;
    RotationError=Links[MapingName2ID.value(current)-1].AttitudeInWorldCoordinate.transpose()*Target.AttitudeInWorldCoordinate;
    AttitudeError=Links[MapingName2ID.value(current)-1].AttitudeInWorldCoordinate*Rot2omega(RotationError);


    TaskSpaceError<<PositionError,AttitudeError;

    return TaskSpaceError;
}

MatrixXd Robot::CalcJacobian(MatrixXi route )
{
    MatrixXd Jacobian;
    Vector3d target;
    Jacobian.fill(0);
    target.fill(0);
    Vector3d JointAxisInWorldFrame;
    JointAxisInWorldFrame.fill(0);
    int JacobianColumnSize;
    JacobianColumnSize=0;
    target=Links[route(route.cols()-1)-1].PositionInWorldCoordinate;
    JacobianColumnSize=route.cols();
    Jacobian= MatrixXd::Zero(6,JacobianColumnSize);
    for (int var = 0; var < JacobianColumnSize; var++) {
        //note var starts from 0
        int mother=Links[route(var)-1].GetMotherID();
        JointAxisInWorldFrame=Links[mother-1].AttitudeInWorldCoordinate*Links[route(var)-1].GetJointAxisVectorLocal();
        Jacobian.col(var)<<JointAxisInWorldFrame.cross(target-Links[route(var)-1].PositionInWorldCoordinate),JointAxisInWorldFrame;
    }

    return Jacobian;
}




double Robot::IKLevenbergMarquardt(QString from,LinkM Target, QString current)
{
    MatrixXd error;
    error.fill(0);
    MatrixXd gerr;
    gerr.fill(0);
    MatrixXd dq;
    dq.fill(0);
    MatrixXd Jh;
    Jh.fill(0);
    MatrixXd J;
    J.fill(0);
    MatrixXd EK2;
    EK2.fill(0);
    MatrixXd EK;
    EK.fill(0);
    double Ek=0;
    double Ek2=0;
    MatrixXi route=FindRoutfromBeginning2Destination(from,current);
    double wn_position =1/0.3;
    double wn_angle = 1/(2*M_PI);
    MatrixXd we(6,6);
    we.fill(0);
    MatrixXd wn;
    we.diagonal()<<wn_position,wn_position,wn_position,wn_angle,wn_angle,wn_angle;
    wn=MatrixXd::Identity(route.cols(),route.cols());
    ForwardKinematic(1);
    ForwardKinematicCoM(1);
    error=CalcTaskSpaceError(Target,current);
    EK=error.transpose()*we*error;
    Ek=EK(0);
    for (int var = 0; var < 10; ++var) {
        J=CalcJacobian(route);

        Jh=J.transpose()*we*J+wn*(Ek+0.02);
        gerr=J.transpose()*we*error;
        dq=Jh.lu().solve(gerr);
        // qDebug()<<dq(0),dq(1),dq(2),dq(3),dq(4),dq(5);
        MoveJoints(route,dq);
        ForwardKinematic(1);
        ForwardKinematicCoM(1);
        error=CalcTaskSpaceError(Target,current);
        EK2=error.transpose()*we*error;
        Ek2=EK2(0);
        if (Ek2<(1e-12)) {
            return error.norm();
        } else if (Ek2 < Ek) {
            Ek = Ek2;
        }

        else {
            MoveJoints(route,-dq);
            ForwardKinematic(1);
            ForwardKinematicCoM(1);
            return error.norm();
        }

    }

    return error.norm();


}

MatrixXd Robot::IKAnalytical(LinkM Body, double D, double E, double A, double B, LinkM Foot)
{
    ForwardKinematic(1);
    MatrixXd DMatrix(3,1);

    MatrixXd EMatrix(3,1);
    DMatrix<<0,D,0;
    EMatrix<<0,0,E;
    MatrixXd Q(6,1);
    double q1,q2,q3,q4,q5,q6,q6a,q7;
    MatrixXd r=Foot.AttitudeInWorldCoordinate.transpose()*(Body.PositionInWorldCoordinate+Body.AttitudeInWorldCoordinate*DMatrix+Body.AttitudeInWorldCoordinate*EMatrix-Foot.PositionInWorldCoordinate);
    double C=r.norm();
    double c5=(C*C-A*A-B*B)/(2*A*B);
    if (c5>=1) {
        q5=0;
        //cout<<"c5 is larger than 1";
    }
    else if (c5<=-1) {
        q5=M_PI;
    }
    else{

        q5=acos(c5); //Knee Pitch
    }
    q6a=asin((A/C)*sin(M_PI-q5));//Ankle Pitch
    q7=atan2(r(1,0),r(2,0));//Ankle Roll
    if (q7>1*M_PI/2) {
        q7=q7-M_PI;
    }
    else if (q7<-1*M_PI/2) {
        q7=q7+M_PI;
    }
    double x=r(0,0);
    double y=Sign(r(2,0))*sqrt(pow(r(1,0),2)+pow(r(2,0),2));
    q6=-1*atan2(x,y)-q6a;//ankle pitch
    MatrixXd Rpitch=RPitch(-1*q5-1*q6);
    MatrixXd Rroll=RRoll(-1*q7);
    MatrixXd R = Body.AttitudeInWorldCoordinate.transpose()*Foot.AttitudeInWorldCoordinate*Rroll*Rpitch; // hipZ*hipX*hipY
    q2=atan2(-1*R(0,1),R(1,1));
    double cz=cos(q2);
    double sz=sin(q2);
    q3=atan2(R(2,1),-1*R(0,1)*sz+R(1,1)*cz);
    q4=atan2(-1*R(2,0),R(2,2));
    Q<<q2,q3,q4,q5,q6,q7;
    //qDebug()<<q2<<q3<<q4<<q5<<q6<<q7;
    return Q;

}


//function rot = RPY2R(rpy)

//roll = rpy(1); pitch = rpy(2); yaw = rpy(3);
//Cphi = cos(roll);  Sphi = sin(roll);
//Cthe = cos(pitch); Sthe = sin(pitch);
//Cpsi = cos(yaw);   Spsi = sin(yaw);

//rot = [Cpsi*Cthe  -Spsi*Cphi+Cpsi*Sthe*Sphi  Spsi*Sphi+Cpsi*Sthe*Cphi;
//       Spsi*Cthe  Cpsi*Cphi+Spsi*Sthe*Sphi   -Cpsi*Sphi+Spsi*Sthe*Cphi;
//       -Sthe      Cthe*Sphi                  Cthe*Cphi                 ];

MatrixXd Robot::RPY2R(Matrix3d rpy){
    MatrixXd rot(3,3);

    double roll = rpy(0,0);
    double pitch = rpy(1,0);
    double yaw = rpy(2,0);
    double Cphi = cos(roll);
    double Sphi = sin(roll);
    double Cthe = cos(pitch);
    double Sthe = sin(pitch);
    double Cpsi = cos(yaw);
    double Spsi = sin(yaw);
    rot<<Cpsi*Cthe ,-Spsi*Cphi+Cpsi*Sthe*Sphi,Spsi*Sphi+Cpsi*Sthe*Cphi,Spsi*Cthe,Cpsi*Cphi+Spsi*Sthe*Sphi,-Cpsi*Sphi+Spsi*Sthe*Cphi,-Sthe,Cthe*Sphi,Cthe*Cphi  ;
    return rot;
}


void Robot::doIKHand(QString from,int var,QString link){
    VectorXd x;
    x.setLinSpaced(3500,0,0.45);
    VectorXd y;
    y.setLinSpaced(3500,-0.31050,-0.33);
    VectorXd z;
    z.setLinSpaced(3500,0.7715,1);//1.38

    VectorXd roll;
    roll.setLinSpaced(3500,0,1.57);
    VectorXd pitch;
    pitch.setLinSpaced(3500,0,1.57);
    VectorXd yaw;
    yaw.setLinSpaced(3500,0,1.57);
    MatrixXd mm(3,1);
    mm<<0*roll(var),-1*pitch(var),-0*yaw(var);


    LinkM EndEffector=Links[MapingName2ID.value(link)-1];
    EndEffector.PositionInWorldCoordinate(0)=x(var);//just specify X
    //EndEffector.PositionInWorldCoordinate(1)=y(var);
    // EndEffector.PositionInWorldCoordinate(2)=z(var);
    // EndEffector.AttitudeInWorldCoordinate=RPY2R(mm);
    double e= IKLevenbergMarquardt(from,EndEffector,link);

    //qDebug()<<Links[1].JointAngle<<Links[2].JointAngle<<Links[3].JointAngle<<Links[4].JointAngle<<Links[5].JointAngle<<Links[6].JointAngle<<Links[7].JointAngle<<Links[8].JointAngle<<Links[9].JointAngle<<Links[10].JointAngle<<Links[11].JointAngle<<Links[12].JointAngle;
}



void Robot::doIK(QString link,MatrixXd PoseLink,QString root,MatrixXd PoseRoot){

    LinkM foot=Links[MapingName2ID.value(link)-1];
    Links[MapingName2ID.value(root)-1].PositionInWorldCoordinate(0)=PoseRoot(0,0);
    Links[MapingName2ID.value(root)-1].PositionInWorldCoordinate(1)=PoseRoot(1,0);
    Links[MapingName2ID.value(root)-1].PositionInWorldCoordinate(2)=PoseRoot(2,0);
    //    Links[MapingName2ID.value(root)-1].AttitudeInWorldCoordinate(0)=PoseRoot(4,0);
    //    Links[MapingName2ID.value(root)-1].AttitudeInWorldCoordinate(1)=PoseRoot(4,0);
    // Links[MapingName2ID.value(root)-1].AttitudeInWorldCoordinate=PoseRoot(5,0);



    foot.PositionInWorldCoordinate(0)=PoseLink(0,0);
    foot.PositionInWorldCoordinate(1)=PoseLink(1,0);
    foot.PositionInWorldCoordinate(2)=PoseLink(2,0);//just specify X
    MatrixXd mm(3,1);
    mm<<PoseLink(3,0),PoseLink(4,0),PoseLink(5,0);
    foot.AttitudeInWorldCoordinate=RPY2R(mm);
    //foot.AttitudeInWorldCoordinate(0)=PoseLink(4,0);
    //foot.AttitudeInWorldCoordinate(0)=PoseLink(5,0);
    //qDebug()<<x(var);
    //double milad=IKLevenbergMarquardt(foot,link);
    MatrixXi Route;
    double D;
    if (link=="LLeg_AnkleR_J6") {
        D=0.115;
        Route= GetLeftLegRoute();
    }
    else if(link=="RLeg_AnkleR_J6") {
        D=-1*0.115;
        Route= GetRightLegRoute();
    }
    MatrixXd Q= IKAnalytical(Links[MapingName2ID.value(root)-1],D,-0.109,0.37,0.36,foot);
    MatrixXd LegAngle(1,6);
    LegAngle<<Q(0,0) ,Q(1,0) ,Q(2,0),Q(3,0), Q(4,0), Q(5,0);
    SetJointAngle(LegAngle,Route);
}



void Robot::doIKOnline(QString link,MatrixXd PoseLink,MatrixXd OrientationLink,QString root,MatrixXd PoseRoot){

    LinkM foot=Links[MapingName2ID.value(link)-1];
    Links[MapingName2ID.value(root)-1].PositionInWorldCoordinate(0)=PoseRoot(0,0);
    Links[MapingName2ID.value(root)-1].PositionInWorldCoordinate(1)=PoseRoot(1,0);
    Links[MapingName2ID.value(root)-1].PositionInWorldCoordinate(2)=PoseRoot(2,0);
    //    Links[MapingName2ID.value(root)-1].AttitudeInWorldCoordinate(0)=PoseRoot(4,0);
    //    Links[MapingName2ID.value(root)-1].AttitudeInWorldCoordinate(1)=PoseRoot(4,0);
    // Links[MapingName2ID.value(root)-1].AttitudeInWorldCoordinate=PoseRoot(5,0);



    foot.PositionInWorldCoordinate(0)=PoseLink(0,0);
    foot.PositionInWorldCoordinate(1)=PoseLink(1,0);
    foot.PositionInWorldCoordinate(2)=PoseLink(2,0);//just specify X
//    MatrixXd mm(3,1);
//    mm<<PoseLink(3,0),PoseLink(4,0),PoseLink(5,0);
//    foot.AttitudeInWorldCoordinate=OrientationLink;
    //foot.AttitudeInWorldCoordinate(0)=PoseLink(4,0);
    //foot.AttitudeInWorldCoordinate(0)=PoseLink(5,0);
    //qDebug()<<x(var);
    //double milad=IKLevenbergMarquardt(foot,link);
    MatrixXi Route;
    double D;
    if (link=="LLeg_AnkleR_J6") {
        D=0.115;
        Route= GetLeftLegRoute();
    }
    else if(link=="RLeg_AnkleR_J6") {
        D=-1*0.115;
        Route= GetRightLegRoute();
    }
    MatrixXd Q= IKAnalytical(Links[MapingName2ID.value(root)-1],D,-0.109,0.37,0.36,foot);
    MatrixXd LegAngle(1,6);
    LegAngle<<Q(0,0) ,Q(1,0) ,Q(2,0),Q(3,0), Q(4,0), Q(5,0);
    SetJointAngle(LegAngle,Route);
}




void Robot::doIKhipRollModify(QString link,MatrixXd PoseLink,QString root,MatrixXd PoseRoot,double rollAngle){//modifying hip roll for pelvis deflection compensation

    LinkM foot=Links[MapingName2ID.value(link)-1];
    Links[MapingName2ID.value(root)-1].PositionInWorldCoordinate(0)=PoseRoot(0,0);
    Links[MapingName2ID.value(root)-1].PositionInWorldCoordinate(1)=PoseRoot(1,0);
    Links[MapingName2ID.value(root)-1].PositionInWorldCoordinate(2)=PoseRoot(2,0);
    //    Links[MapingName2ID.value(root)-1].AttitudeInWorldCoordinate(0)=PoseRoot(4,0);
    //    Links[MapingName2ID.value(root)-1].AttitudeInWorldCoordinate(1)=PoseRoot(4,0);
    // Links[MapingName2ID.value(root)-1].AttitudeInWorldCoordinate=PoseRoot(5,0);



    foot.PositionInWorldCoordinate(0)=PoseLink(0,0);
    foot.PositionInWorldCoordinate(1)=PoseLink(1,0);
    foot.PositionInWorldCoordinate(2)=PoseLink(2,0);//just specify X
    MatrixXd mm(3,1);
    mm<<PoseLink(3,0),PoseLink(4,0),PoseLink(5,0);
    foot.AttitudeInWorldCoordinate=RPY2R(mm);
    //foot.AttitudeInWorldCoordinate(0)=PoseLink(4,0);
    //foot.AttitudeInWorldCoordinate(0)=PoseLink(5,0);
    //qDebug()<<x(var);
    //double milad=IKLevenbergMarquardt(foot,link);
    MatrixXi Route;
    double D;
    if (link=="LLeg_AnkleR_J6") {
        D=0.115;
        Route= GetLeftLegRoute();
    }
    else if(link=="RLeg_AnkleR_J6") {
        D=-1*0.115;
        Route= GetRightLegRoute();
    }
    MatrixXd Q= IKAnalytical(Links[MapingName2ID.value(root)-1],D,-0.109,0.37,0.36,foot);
    MatrixXd LegAngle(1,6);
    LegAngle<<Q(0,0) ,Q(1,0)-rollAngle ,Q(2,0),Q(3,0), Q(4,0), Q(5,0);
    SetJointAngle(LegAngle,Route);

    //qDebug()<<2;

}
