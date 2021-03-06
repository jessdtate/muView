#include <GL/glew.h>

#include <muView/RenderEngine3D.h>
#include <GL/oglCommon.h>
#include <QMouseEvent>

RenderEngine3D::RenderEngine3D(QObject *parent) : QGLWidget() {
    need_view_update = true;
    memset(clpX,0,sizeof(double)*4);
    memset(clpY,0,sizeof(double)*4);
    memset(clpZ,0,sizeof(double)*4);
    clpX[0] = clpY[1] = clpZ[2] = -1;
    useClipX = useClipY = useClipZ = false;
    proj.Set( 40.0f,40.0f, 0.1f,15.0f);
    mouse_active = false;
    parallel_coordinates = 0;


    mouse_button = Qt::NoButton;
    mouse_x = 0;
    mouse_y = 0;

    setMouseTracking(true);

    pln[0] = pln[1] = pln[2] = 0;

}


void RenderEngine3D::initializeGL(){
    glewInit( );
}

void RenderEngine3D::paintGL(){

    // Clear the display
    glClearColor(1,1,1,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    Draw( size().width(), size().height() );

}

void RenderEngine3D::mouseDoubleClickEvent ( QMouseEvent * event ){

    int x = event->x();
    int y = size().height()-event->y()-1;

    MouseClick( event->button(), QMouseEvent::MouseButtonDblClick, x, y );

    mouse_button = event->button();
    mouse_x = x;
    mouse_y = y;

}

void RenderEngine3D::mouseMoveEvent ( QMouseEvent * event ){

    int x = event->x();
    int y = size().height()-event->y()-1;

    MouseMotion( mouse_button, x, x-mouse_x, y, y-mouse_y );

    mouse_x = x;
    mouse_y = y;

}

void RenderEngine3D::mousePressEvent ( QMouseEvent * event ){

    int x = event->x();
    int y = size().height()-event->y()-1;

    MouseClick( event->button(), QMouseEvent::MouseButtonPress, x, y );

    mouse_button = event->button();
    mouse_x = x;
    mouse_y = y;

}

void RenderEngine3D::mouseReleaseEvent ( QMouseEvent * event ){

    int x = event->x();
    int y = size().height()-event->y()-1;

    MouseClick( event->button(), QMouseEvent::MouseButtonRelease, x, y );

    mouse_button = Qt::NoButton;
    mouse_x = x;
    mouse_y = y;

}

void RenderEngine3D::keyPressEvent ( QKeyEvent * event ){

    Keypress( event->key(), mouse_x, mouse_y );

}

void RenderEngine3D::keyReleaseEvent ( QKeyEvent * event ){

}

void RenderEngine3D::UpdateView( ){
    float scl = 2.0f / pmesh->bb.GetMaximumDimensionSize();
    tform.LoadIdentity();
    tform *= SCI::Mat4( SCI::Mat4::MAT4_SCALE, scl, scl, scl );
    tform *= SCI::Mat4( SCI::Mat4::MAT4_TRANSLATE, -pmesh->bb.GetCenter() );

    if( draw_mode == 2 ){
        iso_tris->SortByPainters( iso_tris_ro, proj.GetMatrix() * pView->GetView() * tform, *iso_points );
    }
    if( draw_mode == 3 ){
        df_tris->SortByPainters( df_tris_ro, proj.GetMatrix() * pView->GetView() * tform, *df_points );
    }
    /*
    if(draw_mode == 4){
        edge_mesh->SortByPainters( edge_mesh_ro, proj.GetMatrix() * pView->GetView() * tform, *pmesh );
    }
    */
    tdata->tri_mesh.SortByPainters( tri_mesh_ro, proj.GetMatrix() * pView->GetView() * tform, *pmesh );
    for(int i = 0; i < addl_sil_mesh.size(); i++){
        addl_solid[i]->tri_mesh.SortByPainters( *(addl_mesh_ro[i]), proj.GetMatrix() * pView->GetView() * tform, *(addl_points[i]) );
    }


    need_view_update = false;
}

void RenderEngine3D::Draw( int width, int height ){
    if(pmesh == 0 || tdata == 0) return;

    proj.Set( proj.GetHFOV(), width, height, proj.GetHither(), proj.GetYon() );

    if( need_view_update ){
        UpdateView();
    }

    if( cluster->isRunning() ){
        colormap->resize(pdata->GetVoxelCount());
        for(int i = 0; i < pdata->GetVoxelCount(); i++){
            colormap->at(i) = cat_cmap->GetColor( cluster->GetClusterID(i) ).UIntColor();
        }
        parallel_coordinates->Reset();
    }

    glViewport( 0,0,width,height );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMultMatrixf( proj.GetMatrix().data );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glMultMatrixf( pView->GetView().data );

    if( useClipX){ glEnable( GL_CLIP_PLANE0 ); glClipPlane( GL_CLIP_PLANE0, clpX ); }
    if( useClipY){ glEnable( GL_CLIP_PLANE1 ); glClipPlane( GL_CLIP_PLANE1, clpY ); }
    if( useClipZ){ glEnable( GL_CLIP_PLANE2 ); glClipPlane( GL_CLIP_PLANE2, clpZ ); }

    glMultMatrixf( tform.data );


    glLineWidth(1.0f);


    if(draw_mode == 0){
        glEnable(GL_DEPTH_TEST);
            glPointSize(3.0f);
            if( pdata == 0 ){
                pmesh->Draw( SCI::Vex4(1.0f, 0.95f, 1.0f, 1.0f ) );
            }
            else{
                pmesh->Draw( *colormap );
            }
            //glPointSize(4.0f);
            //pmesh->Draw( SCI::Vex4(0,0,0,1) );
        glDisable(GL_DEPTH_TEST);
    }

    /*
    if(draw_mode == 4){
        glLineWidth(4.0f);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glEnable( GL_BLEND );
            edge_mesh->Draw( edge_mesh_ro, *pmesh, *colormap );
        glDisable( GL_BLEND );
    }
    */




    if( draw_mode == 2 ){
        glDisable(GL_DEPTH_TEST);
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glEnable( GL_BLEND );
            iso_tris->Draw( iso_tris_ro, *iso_points, *iso_color );
        glDisable( GL_BLEND );
    }

    if( draw_mode == 3 ){
        glDisable(GL_DEPTH_TEST);
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glEnable( GL_BLEND );
            df_tris->Draw( df_tris_ro, *df_points, *df_color );
        glDisable( GL_BLEND );
    }

    glDisable(GL_DEPTH_TEST);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_BLEND );
        RecalculateSil();
        tdata->tri_mesh.Draw( tri_mesh_ro, *pmesh, sil_color );
        for(int i = 0; i < addl_sil_mesh.size(); i++){
            addl_solid[i]->tri_mesh.Draw( *(addl_mesh_ro[i]), *(addl_points[i]), *(addl_sil_color[i]) );
        }
    glDisable( GL_BLEND );



    /*
    if( view_fibers ){
        glEnable( GL_DEPTH_TEST );

        glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
            glDepthRange(0.10f,1.0f);
                tdata->tri_mesh.Draw( tri_mesh_ro, *pmesh, SCI::Vex4(0,0,0,0) );
            glDepthRange(0.0f,1.0f);
        glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

        glLineWidth( 3.0f );
        glColor3f(0,0,0);
        glBegin(GL_LINES);
        if(fdata){
            for(int i = 0; i < (int)tdata->tri_mesh.size(); i++){
                Data::Mesh::Triangle t = tdata->tri_mesh.at(i);
                int j = t.v0;
                SCI::Vex3 p0 = pmesh->points[j];
                SCI::Vex3 d  = fdata->fibs[j];
                SCI::Vex3 p1 = p0 + d * fiber_length;

                glVertex3fv( p0.data );
                glVertex3fv( p1.data );
            }
        }
        glEnd();

    }
    */


    glDisable( GL_CLIP_PLANE0 );
    glDisable( GL_CLIP_PLANE1 );
    glDisable( GL_CLIP_PLANE2 );


    /*
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable(GL_DEPTH_TEST);
    glLineWidth(5.0f);
    float max_dim = pmesh->bb.GetMaximumDimensionSize()/2.0f*1.05f;
    SCI::Vex3 cen = pmesh->bb.GetCenter();
    for(int pass = 0; pass <= 1; pass++){
        for(int i = 0; i < 3; i++){
            if( pln[i] == 0 ) continue;
            if(pass == 0) glEnable( GL_BLEND );
            if(pass == 1) glDisable( GL_BLEND );
            if(pass == 0) glBegin(GL_QUADS);
            if(pass == 1) glBegin(GL_LINE_LOOP);
            glColor4f(pln_color[i]->x,pln_color[i]->y,pln_color[i]->z,0.4f);
            if( pln[i]->x != 0 ){
                glVertex3f(-pln[i]->w,cen.y-max_dim,cen.z-max_dim);
                glVertex3f(-pln[i]->w,cen.y-max_dim,cen.z+max_dim);
                glVertex3f(-pln[i]->w,cen.y+max_dim,cen.z+max_dim);
                glVertex3f(-pln[i]->w,cen.y+max_dim,cen.z-max_dim);
            }
            if( pln[i]->y != 0 ){
                glVertex3f(cen.x-max_dim,-pln[i]->w,cen.z-max_dim);
                glVertex3f(cen.x-max_dim,-pln[i]->w,cen.z+max_dim);
                glVertex3f(cen.x+max_dim,-pln[i]->w,cen.z+max_dim);
                glVertex3f(cen.x+max_dim,-pln[i]->w,cen.z-max_dim);
            }
            if( pln[i]->z != 0 ){
                glVertex3f(cen.x-max_dim,cen.y-max_dim,-pln[i]->w);
                glVertex3f(cen.x-max_dim,cen.y+max_dim,-pln[i]->w);
                glVertex3f(cen.x+max_dim,cen.y+max_dim,-pln[i]->w);
                glVertex3f(cen.x+max_dim,cen.y-max_dim,-pln[i]->w);
            }
            glEnd();
        }
    }
    glDisable(GL_DEPTH_TEST);
    glDisable( GL_BLEND );
    */

    glLineWidth(1.0f);

    // Now othrographic mode drawing
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    if(color_mode != 3){
        seq_cmap->Draw( 0.8f, 0.875f, 0.15f, 0.9f, (float)width/(float)height, font );
    }

    if(color_mode == 3){

        float vmin =  FLT_MAX;
        float vmax = -FLT_MAX;
        for(int i = 0; i < cluster->GetClusterCount(); i++){
            std::vector<float> clstr = cluster->GetCluster(i);
            for(int j = 0; j < (int)clstr.size(); j++){
                vmin = SCI::Min(vmin,clstr[j]);
                vmax = SCI::Max(vmax,clstr[j]);
            }
        }
        vmax = (vmax-vmin)*1.1f+vmin;

        std::vector<Histogram> histograms;

        for(int i = 0; i < cluster->GetClusterCount(); i++){
            std::vector<float> clstr = cluster->GetCluster(i);
            histograms.push_back( Histogram() );
            histograms.back().Reinitialize( 10, vmin, vmax );
            histograms.back().SetColor( cat_cmap->GetColor( i ).xyz() );
            //histograms.back().ClearBins();
            for(int j = 0; j < (int)clstr.size(); j++){
                histograms.back().AddValue( clstr[j] );
            }

        }

        std::sort( histograms.begin(), histograms.end() );

        for(int i = 0; i < (int)histograms.size(); i++){
            float u0 = 0.8f;
            float u1 = 1.0f;
            float v0 = 2.0f * (float)(i+0)/(float)histograms.size() - 1.0f;
            float v1 = 2.0f * (float)(i+1)/(float)histograms.size() - 1.0f;

            if(cluster_histogram){
                histograms[i].Draw( u0, v0, u1, v1, pdata->GetDim(), ((i==0)?font:0), (float)width/(float)height );
            }
            else{
                histograms[i].DrawValues( u0, v0, u1, v1 );
            }

        }
    }

}


void RenderEngine3D::RecalculateSil(){
    if( tdata != 0 && pmesh != 0 ){
        if( sil_mesh.size() == 0 ){
            sil_mesh.Calculate( tdata->tri_mesh, *pmesh );
            //sil_mesh.Smooth( tdata->tri_mesh, 1 );
            sil_mesh.ZeroFlatRegions(tdata->tri_mesh, *pmesh );
        }
        sil_color.SetFromOrientation( sil_mesh, pView->GetVD(), 7.5f );
    }

    for(int i = 0; i < (int)addl_points.size() && i < (int)addl_solid.size(); i++){
        if( (int)addl_sil_mesh.size()  <= i ){ addl_sil_mesh.push_back( new Data::Mesh::OrientationMesh() ); }
        if( (int)addl_sil_color.size() <= i ){ addl_sil_color.push_back( new Data::Mesh::ColorMesh() ); }
        if( (int)addl_mesh_ro.size()   <= i ){ addl_mesh_ro.push_back( new std::vector<int>() ); }
        if( (int)addl_sil_mesh[i]->size() == 0 ){
            addl_sil_mesh[i]->Calculate( addl_solid[i]->tri_mesh, *(addl_points[i]) );
            //addl_sil_mesh[i]->Smooth( tdata->tri_mesh, 1 );
            addl_sil_mesh[i]->ZeroFlatRegions(addl_solid[i]->tri_mesh, *(addl_points[i]) );
        }
        addl_sil_color[i]->SetFromOrientation( *(addl_sil_mesh[i]), pView->GetVD(), 2.0f );
    }
}

bool RenderEngine3D::MouseClick(int button, int state, int x, int y){
    pView->Save("view.txt");
    mouse_active = ( state == QMouseEvent::MouseButtonPress );
    return mouse_active;
}


bool RenderEngine3D::MouseMotion(int button, int x, int dx, int y, int dy){
    if(mouse_active){
        if(button == Qt::LeftButton)   pView->Rotate(-(float)(dx),(float)(dy));
        if(button == Qt::MiddleButton) pView->Translate((float)(dx),(float)(dy));
        if(button == Qt::RightButton)  pView->Zoom((float)(dy));
        need_view_update = true;
        update();
    }
    return mouse_active;
}

bool RenderEngine3D::Keypress( int key, int x, int y ){
    return false;
}


void RenderEngine3D::Initialize( ){ }

