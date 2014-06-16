//-------------------------------------------------------------------
//  File:     HObject.cpp
//  Created:  September 09, 2000
//  Author:   Jeremie Allard (Hufo / N.A.A.)
//  Comments: Create and manage Objects.
//-------------------------------------------------------------------
#include "stdafx.h"
#include "typdefi.h"
#include "std.h"
#include "HDef.h"
#include "HObject.h"
#include "IOFile.h"
#include "imgLoadBMP.h" // Image loader handler for RTexture/RImage.

extern PFNGLLOCKARRAYSEXTPROC    glLockArraysEXT;
extern PFNGLUNLOCKARRAYSEXTPROC  glUnlockArraysEXT;

bool HMaterial::UseArray=true;
#define HMsgLWO(s) 

HObject::HObject()
: nbf(0),nbp(0),fc(NULL),pt(NULL)
{}

HObject::~HObject()
{
	Free();
}

void HObject::Free()
{
	if (fc) { delete[] fc; fc=NULL; }
	if (pt) { delete[] pt; pt=NULL; }
	nbf=0; nbp=0;
}

template<class T>
inline bool HFRead(IOFile &h,T *p)
{
	h >> *p;
	return r_ok;
}

inline int HFPos(IOFile &h)
{
	return h.GetPosition();
}

#define HFD_DEBUT   1
#define HFD_COURANT 2
#define HFD_FIN     4

inline void HFDepl(IOFile &h,int n,int mode=HFD_COURANT)
{
	switch (mode)
	{
		case HFD_DEBUT:
			h.SeekFromStart(n);
			break;
		case HFD_COURANT:
			h.SeekFromHere(n);
			break;
		case HFD_FIN:
			h.SeekFromEnd(n);
			break;
	}
}

bool HFReadLWOStr(IOFile &h,char *s,int *l)
{ //lecture d'une chaine de caractères
	for (;;)
	{
		Test(h.Read(s,2));
		if (l!=NULL) *l-=2;
		if (s[0]==0 || s[1]==0) break;
		s+=2;
	}
	return r_ok;
}

bool HObject::LoadLWO(char* szFileName)
{
	IOFile h;
	Test(h.Create(szFileName,"rb"));
	int length,l;
	union
	{
		DWord id;
		char sid[5];
	};
	sid[4]=0;
	Word w,nw;
	int i,j;
	float f;
	int sp;
	char buf[256];
	HMatTexture *curtxt;
	h >> id; //Test(HFRead(h,&id));
	if (id!='MROF') return HErrorN(HText("Bad RIFF FORM id:%x",id));
	h >> length; //Test(HFRead(h,&length));
	length=dswap(length);
	h >> id; //Test(HFRead(h,&id));
	length-=sizeof(id);
	if (id!='BOWL') return HErrorN(HText("Bad lightwave id:%x",id));
	Free();
	HSurface *ss=NULL;
	while (length)
	{
		Test(HFRead(h,&id));            length-=sizeof(id);
		Test(HFRead(h,&l)); l=dswap(l); length-=sizeof(l);
		if (l>length) return HErrorN(HText("Bad chunk size: end after EOF (id=%s),diff=%d",sid,l-length));
		HMsgLWO(HText("chunk: %x(%s) size=%d",id,sid,l));
		length-=l;
		switch(id)
		{
			case 'SFRS' : // SRFS
				if (nbs) return HErrorN("2 SRFS chunks");
				sp=HFPos(h);
				while (l)
				{
					Test(HFReadLWOStr(h,buf,&l));
					if (l<0) return HErrorN(HText("Bad chunk size: %d bytes missing",-l));
					++nbs;
				}
				HFDepl(h,sp,HFD_DEBUT);
				if (nbs)
				{
					sf=new HSurface[nbs];
					for (j=0;j<nbs;++j)
					{
						Test(HFReadLWOStr(h,sf[j].name,NULL));
						HMsgLWO(HText("new material:%s",sf[j].name));
					}
				}
				break;
			case 'FRUS' : // SURF
			{
				if (!nbs) return HErrorN("no surface");
				Test(HFReadLWOStr(h,buf,&l));
				if (l<0) return HErrorN("Bad chunk size");
				for (j=0;j<nbs;++j)
					if (!stricmp(buf,sf[j].name)) break;
				if (j==nbs) return HErrorN(HText("unknown LW surface:%s",buf));
				ss=&sf[j];
				ss->flag=0;
				curtxt=NULL;
				int ll;
				HColor4f colr(0.5f,0.5f,0.5f);
				float lumi=0;
				float diff=1.0f;
				float spec=0;
				int glos=64;
				float refl=0;
				float trans=0;
				Byte byte;
				while (l)
				{
					Test(HFRead(h,&id));              l-=sizeof(id);
					Test(HFRead(h,&w )); ll=wswap(w); l-=sizeof(w );
					//Test(HMsg(HText("subchunk %s, size %d",sid,ll)));
					if (ll>l) return HErrorN(HText("Bad chunk size: end after EOF (id=%s),diff=%d %x",sid,ll-l,ll));
					l-=ll;
					switch(id)
					{
						case 'GALF' : //FLAG
							if (ll!=2) return HErrorN("invalid FLAG subchunk size");
							Test(HFRead(h,&w )); w=wswap(w); ll-=sizeof(w );
							if (w&4) ss->flag|=HMaterial::Smooth;
							//if (w&256) ss->flag|=HMaterial::Double;
							if (w&512) ss->flag|=HMaterial::Blend|HMaterial::Additive;
							break;
						case 'RLOC' : // COLR
							if (ll!=4) return HErrorN("invalid COLR subchunk size");
							Test(HFRead(h,&byte)); colr.r=byte*(1.0f/255.0f);
							Test(HFRead(h,&byte)); colr.g=byte*(1.0f/255.0f);
							Test(HFRead(h,&byte)); colr.b=byte*(1.0f/255.0f);
							Test(HFRead(h,&byte));
							break;
						case 'IMUL' : // LUMI
							if (ll!=2) return HErrorN("invalid LUMI subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							lumi=w*(1.0f/255.0f);
							break;
						case 'FFID' : // DIFF
							if (ll!=2) return HErrorN("invalid DIFF subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							diff=w*(1.0f/255.0f);
							break;
						case 'CEPS' : // SPEC
							if (ll!=2) return HErrorN("invalid SPEC subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							spec=w*(1.0f/255.0f);
							break;
						case 'SOLG' : // GLOS
							if (ll!=2) return HErrorN("invalid GLOS subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							glos=w;
							break;
						case 'LFER' : // REFL
							if (ll!=2) return HErrorN("invalid REFL subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							refl=w*(1.0f/255.0f);
							break;
						case 'NART' : // TRAN
							if (ll!=2) return HErrorN("invalid TRAN subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							trans=0.5f; //w*(1.0f/255.0f);
							break;
						case 'XETC' : // CTEX (color)
						case 'XETD' : // DTEX (diffuse)
						case 'XETS' : // STEX (specular)
						case 'XETR' : // RTEX (reflection)
						case 'XETT' : // TTEX (transparency)
						case 'XETB' : // BTEX (bump)
							//curtxt=id; //usemap=False;
							if (id=='XETC') curtxt=&ss->ctxt;
							else            curtxt=NULL;
							Test(HFReadLWOStr(h,buf,&ll));
							if (ll) return HErrorN("invalid ?TEX subchunk size");
							if (curtxt)
							{
								if (!strcmp(buf,"Planar Image Map"))
									curtxt->map=HMatTexture::Plan;
								else if (!strcmp(buf,"Cylindrical Image Map"))
									curtxt->map=HMatTexture::Cyl;
								else if (!strcmp(buf,"Spherical Image Map"))
									curtxt->map=HMatTexture::Sphe;
								else return HErrorN("unknown LW mapping");
							}
							break;
						case 'GMIT' : // TIMG
							Test(HFReadLWOStr(h,buf,&ll));
							if (ll) return HErrorN("invalid TIMG subchunk size");
							if (curtxt)
							{
								curtxt->FileName(buf);	
							}
							break;
						case 'GLFT' : // TFLG
							if (ll!=2) return HErrorN("invalid TFLG subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							if (curtxt)
							{
								curtxt->axe=(w&1) ? HMatTexture::AxeX : ((w&2) ? HMatTexture::AxeY : HMatTexture::AxeZ);
							}
							break;
						case 'ZIST' : // TSIZ
							if (ll!=12) return HErrorN("invalid TSIZ subchunk size");
							if (curtxt)
							{
								Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
								curtxt->size.x=f;
								Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
								curtxt->size.y=f;
								Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
								curtxt->size.z=f;
							}
							else
								HFDepl(h,ll,HFD_COURANT);
							break;
						case 'RTCT' : // TCTR
							if (ll!=12) return HErrorN("invalid TCTR subchunk size");
							if (curtxt)
							{
								Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
								curtxt->ctr.x=f;
								Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
								curtxt->ctr.y=f;
								Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
								curtxt->ctr.z=f;
							}
							else
								HFDepl(h,ll,HFD_COURANT);
							break;
						case 'PRWT' : // TWRP
							if (ll!=4) return HErrorN("invalid TWRP subchunk size");
							if (curtxt)
							{
								Test(HFRead(h,&w)); w=wswap(w);
								//curtxt->warpx=(w!=0);
								Test(HFRead(h,&w)); w=wswap(w);
								//curtxt->warpy=(w!=0);
							}
							else
								HFDepl(h,ll,HFD_COURANT);
							break;
						default: //HMsg(HText("unkown SURF:%s",sid)); //cout << "unknown SURF:"<<(char*)&id<<" chunk\n";
							HFDepl(h,ll,HFD_COURANT);
					}
				}
				colr.a=1.0f-trans;
				if (trans) ss->flag|=HMaterial::Blend;
				//lumi=diff=spec=refl=0;
				colr.r=1.0f; colr.g=1.0f; colr.b=1.0f; colr.a=1.0f;
				//lumi=0.5f;
				ss->ambient.r=colr.r*lumi;
				ss->ambient.g=colr.g*lumi;
				ss->ambient.b=colr.b*lumi;
				ss->ambient.a=0; //colr.a;
				ss->diffuse.r=colr.r*diff;
				ss->diffuse.g=colr.g*diff;
				ss->diffuse.b=colr.b*diff;
				ss->diffuse.a=0; //colr.a;
				ss->specular.r=spec;
				ss->specular.g=spec;
				ss->specular.b=spec;
				ss->specular.a=0; //colr.a;
				ss->emission.r=colr.r*refl;
				ss->emission.g=colr.g*refl;
				ss->emission.b=colr.b*refl;
				ss->emission.a=colr.a;
				ss->shininess=glos;
				if (diff || spec || refl) ss->flag|=HMaterial::Lighting;
				else ss->flag&=~HMaterial::Smooth;
				if (ss->flag&HMaterial::Lighting) ss->flag|=HMaterial::DynLighting;
				break;
			}
			case 'STNP' : // PNTS
				if (nbp) return HErrorN("2 PNTS chunks");
				if (l%(3*sizeof(float))) return HErrorN("bad PNTS chunk size");
				nbp=l/(3*sizeof(float));
				HMsgLWO(HText("nbp=%d",nbp));
				if (nbp)
				{
					pt=new HVertex[nbp];
					for (j=0;j<nbp;j++)
					{
						Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
						pt[j].p.x=f;
						Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
						pt[j].p.y=f;
						Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
						pt[j].p.z=f;
					}
				}
				break;
			case 'SLOP' : // POLS
			{
				if (nbf) return HErrorN("2 POLS chunks");
				sp=HFPos(h);
				j=0; nbf=0;
				while (j<l)
				{
					Test(HFRead(h,&w)); w=wswap(w); j+=2;
					HFDepl(h,2*w+2); j+=2*w+2;
					if (w>=3)
						nbf+=w-2;
				}
				if (j>l) return HErrorN(HText("bad POLS chunk:%d!=%d",l,j));
				HMsgLWO(HText("nbf=%d",nbf));
				HFDepl(h,sp,HFD_DEBUT);
				if (nbf)
				{
					fc=new HFace[nbf];
					for (j=0;j<nbf;j++)
					{
						Test(HFRead(h,&nw)); nw=wswap(nw); l-=4+nw*2;
						if (nw<=2) HFDepl(h,nw*2+2);
						else
						{
							Test(HFRead(h,&w)); w=wswap(w);
							if (w>=nbp)
								return HErrorN(HText("LWO:point inexistant:%x (face %d)",w,j));
							fc[j].v[0]=&(pt[w]);
							Test(HFRead(h,&w)); w=wswap(w);
							if (w>=nbp)
								return HErrorN(HText("LWO:point inexistant:%x (face %d)",w,j));
							fc[j].v[1]=&(pt[w]);
							Test(HFRead(h,&w)); w=wswap(w);
							if (w>=nbp)
								return HErrorN(HText("LWO:point inexistant:%x (face %d)",w,j));
							fc[j].v[2]=&(pt[w]);
							i=j;
							while (nw>3)
							{ ++j;--nw;
								fc[j].v[0]=fc[i].v[0]; fc[j].v[1]=fc[j-1].v[2];
								Test(HFRead(h,&w)); w=wswap(w);
								if (w>=nbp)
									return HErrorN(HText("LWO:point inexistant:%x (face %d)",w,j));
								fc[j].v[2]=&(pt[w]);
							}
							Test(HFRead(h,&w)); w=wswap(w);
							if (w==0 || w>nbs) return HErrorN(HText("LWO: surface inexistante:%x",w));
							sf[w-1].nbf+=1+j-i;
							while (i<=j) fc[i++].m=&sf[w-1];
						}
					}
					if (j!=nbf) return HErrorN(HText("bad poly number:%d instead of %d",j,nbf));

					if (nbs)
					{
						// faces and points are reorganised by surfaces
						int **ptpos; // point pos in each surface
						ptpos=new int*[nbs];
						for (j=0;j<nbs;j++)
						{
							ptpos[j]=new int[nbp];
							for (i=0;i<nbp;++i)
								ptpos[j][i]=-1;
							sf[j].nbp=0;
						}
						HSurface *s;
						int pnum,snum;
						for (j=0;j<nbf;++j)
						{
							snum=(HSurface*)(fc[j].m)-sf;
							for (i=0;i<3;++i)
							{
								pnum=fc[j].v[i]-pt;
								if (ptpos[snum][pnum]<0)
									ptpos[snum][pnum]=sf[snum].nbp++;
							}
						}
						int np=0;
						for (j=0;j<nbs;j++)
							np+=sf[j].nbp;

						HVertex *ptb=pt;
						HFace *fb=fc;
						int nbpb=nbp; nbp=np;
						fc=new HFace[nbf];
						pt=new HVertex[nbp];
						i=0;
						np=0;
						for (j=0;j<nbs;++j)
						{
							sf[j].fc=fc+i;
							i+=sf[j].nbf;
							sf[j].pt=pt+np;
							np+=sf[j].nbp;
							sf[j].fc_i=new int[sf[j].nbf*3];
							sf[j].nbf=0;
						}
						for (j=0;j<nbpb;++j)
						{
							for (i=0;i<nbs;++i)
							{
								pnum=ptpos[i][j];
								if (pnum>=0)
								{
									sf[i].pt[pnum]=ptb[j];
								}
							}
						}
						for (j=0;j<nbf;++j)
						{
							s=(HSurface*)fb[j].m;
							snum=s-sf;
							s->fc[s->nbf]=fb[j];
							for (i=0;i<3;++i)
							{
								pnum=fb[j].v[i]-ptb; // ancien numéro
								pnum=ptpos[snum][pnum]; // nouveau numéro
								s->fc_i[s->nbf*3+i]=pnum;
								s->fc[s->nbf].v[i]=s->pt+pnum;
							}
							++s->nbf; // une face a ét?rajoutée dans la surface
						}
						delete[] fb;
						delete[] ptb;
						for (j=0;j<nbs;++j)
							delete[] ptpos[j];
						delete[] ptpos;
					}
				}
				if (l) { HFDepl(h,l); l=0; }
				break;
			}
			default: //cout << "unknown "<<(char*)&id<<" chunk\n";
				HFDepl(h,l,HFD_COURANT);
		}
	}
//	// enfin on calcule les vecteurs normaux
	h.Toast(); //HFClose(h);

	if (nbf)
	{
		HVertex *pv=pt;
		for (j=0;j<nbp;++j,++pv)
		{
			pv->n.Zero();
		}
		HFace *pf=fc;
		for (j=0;j<nbf;++j,++pf)
		{
			pf->CalcNormal();
			pf->v[0]->n+=pf->n;
			pf->v[1]->n+=pf->n;
			pf->v[2]->n+=pf->n;
		}
		pv=pt;
		for (j=0;j<nbp;++j,++pv)
		{
			pv->n.Normalize();
		}
	}
	// ensuite il faut calculer les coords de mapping
	CalcMapping();
	return r_ok;
}

void HVertex::Put(int pdata) const
{
	if (pdata&PUT_TEXCOORD)
		glTexCoord2f(t.u,t.v);
	if (pdata&PUT_COLOR)
		glColor4f(c.r,c.g,c.b,c.a);
	if (pdata&PUT_NORMAL)
		glNormal3f(n.x,n.y,n.z);
	if (pdata&PUT_COORD)
		glVertex3f(p.x,p.y,p.z);
}

void HFace::Put(int pdata) const
{
	//if (pdata&PUT_NORMAL)
		//glNormal3f(n.x,n.y,n.z);
	v[0]->Put(pdata);
	v[1]->Put(pdata);
	v[2]->Put(pdata);
}

void HFace::Aff(int pdata) const
{
	glBegin(GL_TRIANGLES);
		Put(pdata);
	glEnd();
}

void HObject::Aff() const
{
	int j;
	if (nbs)
	for (j=0;j<nbs;++j)
		sf[j].Aff();
}

int HMaterial::Put() const
{
	int pdata=PUT_COORD;
	glShadeModel((flag&Smooth)?GL_SMOOTH:GL_FLAT);
	//if (flag&Double)
		//glDisable(GL_CULL_FACE);
	//else
		glEnable(GL_CULL_FACE);
	if (flag&Blend)
	{
		if (flag&Additive)
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		else
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	}
	else
		glDisable(GL_BLEND);
	if (flag&DynLighting)
	{
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,emission);
		glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,(flag&Double));
		glEnable(GL_LIGHTING);
		pdata|=PUT_NORMAL;
		//diffuse.Put();
	}
	else
	{
		glDisable(GL_LIGHTING);
		ambient.Put();
	}
	if (flag&Color)
		pdata|=PUT_COLOR;
	if (flag&CTxt)
	{
		glEnable( GL_TEXTURE_2D );
		ctxt.Use();
		pdata|=PUT_TEXCOORD;
	}
	else
		glDisable( GL_TEXTURE_2D );
	return pdata;
}

#include <stddef.h>
void HSurface::Aff() const
{
	int pdata=Put(); // configure material
	bool array=false;
	if (pt!=NULL && UseArray)
	switch(pdata)
	{
			case PUT_COORD:
				glDisableClientState(GL_NORMAL_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
				glDisableClientState(GL_EDGE_FLAG_ARRAY);
				glDisableClientState(GL_INDEX_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glInterleavedArrays(GL_V3F,sizeof(HVertex),(Byte*)pt+offsetof(HVertex,p));
				array=true;
				break;
			case PUT_COORD|PUT_NORMAL:
				glDisableClientState(GL_COLOR_ARRAY);
				glDisableClientState(GL_EDGE_FLAG_ARRAY);
				glDisableClientState(GL_INDEX_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glInterleavedArrays(GL_N3F_V3F,sizeof(HVertex),(Byte*)pt+offsetof(HVertex,n));
				array=true;
				break;
			//case PUT_COORD|PUT_COLOR:
				//glInterleavedArrays(GL_C4F_V3F,sizeof(HVertex),(Byte*)pt+offsetof(HVertex,n));
				//array=true;
				//break;
			case PUT_COORD|PUT_NORMAL|PUT_COLOR:
				glDisableClientState(GL_EDGE_FLAG_ARRAY);
				glDisableClientState(GL_INDEX_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glInterleavedArrays(GL_C4F_N3F_V3F,sizeof(HVertex),(Byte*)pt+offsetof(HVertex,c));
				array=true;
				break;
		default:
				if (pdata&PUT_COORD)
				{
					glVertexPointer(3,GL_FLOAT,sizeof(HVertex),(Byte*)pt+offsetof(HVertex,p));
					glEnableClientState(GL_VERTEX_ARRAY);
				}
				else
					glDisableClientState(GL_VERTEX_ARRAY);
				if (pdata&PUT_NORMAL)
				{
					glNormalPointer(GL_FLOAT,sizeof(HVertex),(Byte*)pt+offsetof(HVertex,n));
					glEnableClientState(GL_NORMAL_ARRAY);
				}
				else
					glDisableClientState(GL_NORMAL_ARRAY);
				if (pdata&PUT_COLOR)
				{
					glColorPointer(4,GL_FLOAT,sizeof(HVertex),(Byte*)pt+offsetof(HVertex,c));
					glEnableClientState(GL_COLOR_ARRAY);
				}
				else
					glDisableClientState(GL_COLOR_ARRAY);
				if (pdata&PUT_TEXCOORD)
				{
					glTexCoordPointer(2,GL_FLOAT,sizeof(HVertex),(Byte*)pt+offsetof(HVertex,t));
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				}
				else
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_EDGE_FLAG_ARRAY);
				glDisableClientState(GL_INDEX_ARRAY);
				array=true;
	}
	int i;
	HFace *f;
	f=fc; i=nbf;
	if (array)
	{
// 		if (glExt_compiled_vertex_array.initok)
// 			glLockArraysEXT(0,nbp);
// 		glDrawElements(GL_TRIANGLES,nbf*3,GL_UNSIGNED_INT,fc_i);
// 		if (glExt_compiled_vertex_array.initok)
// 			glUnlockArraysEXT();

		// select the sand texture
		//glBindTexture(GL_TEXTURE_2D, g_sand);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		if (glLockArraysEXT)
			glLockArraysEXT(0,nbp);

		// loop through all the triangle strips
		// draw the triangles in this strip
		glDrawElements(GL_TRIANGLES,nbf*3,GL_UNSIGNED_INT,fc_i);

		// if the compiled arrays extension is available, unlock the arrays
		if (glUnlockArraysEXT)
			glUnlockArraysEXT();
	}
	else
	{
		glBegin(GL_TRIANGLES);
			while (i--)
			{
				f->Put(pdata);
				++f;
			}
		glEnd();
	}
}

void HFace::CalcNormal()
{
	HPoint3D a=v[1]->p - v[0]->p;
	HPoint3D b=v[2]->p - v[0]->p;
	n.CrossProduct(a,b);
	n.Normalize();
}

bool HMaterial::IsLighting() const
{
	return (flag&Lighting)!=0;
}
void HObject::Translate(const HPoint3D &vt)
{
	int i; HVertex *p;
	i=nbp;
	p=pt;
	while (i--)
	{
		p->p+=vt;
		++p;
	}
}

void HVertex::ClearColor(const HColor4f &c0)
{
	c=c0;
}

void HObject::ClearColor(const HColor4f &c0)
{
	HVertex *p=pt;
	int np=nbp;
	while (np--)
	{
		p->ClearColor(c0);
		++p;
	}
}

void HVertex::ApplyLight(const HMaterial &m, const HPoint3D &lp, const HColor4f &lc,const HColor4f &lighta)
{
	c=m.ambient;
	c*=lighta;
	float l;
	if (m.IsLighting())
	{
		l=n*lp;
		if (l>0.0f)
			c.Add(m.diffuse,l);
	}
}

void HSurface::ApplyLight(const HPoint3D &lp, const HColor4f &lc,const HColor4f &lighta)
{
	HVertex *p=pt;
	int np=nbp;
	while (np--)
	{
		p->ApplyLight(*this,lp,lc,lighta);
		++p;
	}
}

void HObject::ApplyLight(const HPoint3D &lp, const HColor4f &lc,const HColor4f &lighta)
{
	HSurface *s=sf;
	int ns=nbs;
	while (ns--)
	{
		s->ApplyLight(lp,lc,lighta);
		++s;
	}
}

bool HMaterial::GetDynLighting()
{
	return (flag&DynLighting)!=0;
}

bool HMaterial::SetDynLighting(bool b)
{
	if (flag&Lighting)
	{
		if (b)
			flag|=DynLighting;
		else
			flag&=~DynLighting;
	}
	return (flag&DynLighting)!=0;
}

bool HMaterial::GetFlagColor()
{
	return (flag&Color)!=0;
}

void HMaterial::SetFlagColor(bool b)
{
	if (b)
		flag|=Color;
	else
		flag&=~Color;
}

void HObject::SetDynLighting(bool b)
{
	if (nbs)
	{
		HSurface *s=sf;
		int ns=nbs;
		while (ns--)
		{
			s->SetDynLighting(b);
			++s;
		}
	}
}

void HObject::SetFlagColor(bool b)
{
	if (nbs)
	{
		HSurface *s=sf;
		int ns=nbs;
		while (ns--)
		{
			s->SetFlagColor(b);
			++s;
		}
	}
}

HMatTexture::HMatTexture()
: name(NULL),txt(NULL),map(None),axe(Axe0),ctr(0,0,0),size(1,1,1)
{
}

HMatTexture::~HMatTexture()
{
	Free();
	if (name) delete[] name;
}

void HMatTexture::FileName(char *fname)
{
	if (name) delete[] name;
	if (fname==NULL) name=NULL;
	else
	{
		int n=strlen(fname);
		name=new char[n+1];
		memcpy(name,fname,n+1);
	}
}

bool HMatTexture::Load()
{
	if (name)
	{
		txt=new RTexture;

		char buf[256];
		strcpy(buf,"data/");
		char *fname=strrchr(name,'\\')+1;
		strcat(buf,fname);

		//return txt->Create(name, imgLoadBMP );
		return txt->Create(buf, imgLoadBMP );
		//return txt->LoadJPG(name);
	}
	else return false;
}

void HMatTexture::Free()
{
	if (txt)
	{
		txt->Toast();
		delete[] txt;
	}
}

void HMatTexture::Use() const
{
	if (txt) txt->Use();
}

bool HMatTexture::Usable() const
{
	return txt && txt->Usable();
}

bool HMatTexture::Loadable() const
{
	return name!=NULL;
}

float LWOCalcCyl(float x,float y)
{
	float a;
	if      (x==0 && y==0) a=0;
	else if (y==0) a=(x<0) ? PI/2 : -PI/2;
	else if (y<0)  a=(float)(-atan(x/y)+PI);
	else             a=(float)(-atan(x/y));
	return 1-a/(2*PI);
}

void LWOCalcSphe(float x,float y,float z,float *u,float *v)
{
	float a,b;
	if (x==0 && z==0)
	{ a=0;
		if (y==0) b=0; else b=(y<0) ? -PI/2 : PI/2;
	}
	else
	{
		if      (z==0) a=(x<0) ? PI/2 : -PI/2;
		else if (z<0 ) a=(float)(-atan(x/z)+PI);
		else             a=(float)(-atan(x/z));
		x=(float)sqrt(x*x+z*z);
		if (x==0) b=(y<0)? -PI/2 : PI/2;
		else        b=(float)atan(y/x);
	}
	*u=1-a/(2*PI);
	*v=0.5f-b/(PI);
}

void HMatTexture::CalcMapping(HVertex *vt) const
{
	MapAxe axe=AxeY;
	float u,v,x,y,z;
	x=vt->p.x-ctr.x;	
	y=vt->p.y-ctr.y;	
	z=vt->p.z-ctr.z;	
	float sx,sy,sz;
	sx=x/size.x+0.5f;
	sy=y/size.y+0.5f;
	sz=z/size.z+0.5f;
	switch (map)
	{
		case Plan:
			u=(axe==AxeX)?  sz :  sx ;
			v=(axe==AxeY)? -sz : -sy ;
			break;
		case Cyl:
			if      (axe==AxeX)
			{ u=LWOCalcCyl( z,-y); v=-sx; }
			else if (axe==AxeY)
			{ u=LWOCalcCyl(-x, z); v=-sy; }
			else
			{ u=LWOCalcCyl(-x,-y); v=-sz; }
			break;
		case Sphe:
			if      (axe==AxeX) LWOCalcSphe( z, x,-y,&u,&v);
			else if (axe==AxeY) LWOCalcSphe(-x, y, z,&u,&v);
			else                LWOCalcSphe(-x, z,-y,&u,&v);
			break;
		default: u=v=0;
	}
	vt->t.u=u; vt->t.v=v;
}

bool HObject::LoadTexture()
{
	bool b=true;
	HSurface *s=sf;
	int ns=nbs;
	while (ns--)
	{
		b|=s->LoadTexture();
		++s;
	}
	return b;
}

void HObject::FreeTexture()
{
	HSurface *s=sf;
	int ns=nbs;
	while (ns--)
	{
		s->FreeTexture();
		++s;
	}
}

bool HMaterial::LoadTexture()
{
	bool b=true;
	if (ctxt.Loadable())
	{
		if (ctxt.Load())
		{
			if (ctxt.Usable())
			{
				//HMsg(HText("texture %s loaded",ctxt.name));
				flag|=CTxt;
			}
		}
		else
			b=false;
	}
	return b;
}

void HMaterial::FreeTexture()
{
	if (ctxt.Usable())
		ctxt.Free();
}

void HMaterial::CalcMapping(HVertex *v) const
{
	if (ctxt.Loadable())
		ctxt.CalcMapping(v);
}

void HSurface::CalcMapping()
{
	if (!ctxt.Loadable()) return;
	HVertex *p=pt;
	int np=nbp;
	while (np--)
	{
		HMaterial::CalcMapping(p);
		++p;
	}
}

void HObject::CalcMapping()
{
	HSurface *s=sf;
	int ns=nbs;
	while (ns--)
	{
		s->CalcMapping();
		++s;
	}
}

void HObject::CalcBBox(HBBox *bb)
{
	if (nbp>0)
	{
		HVertex *v=pt;
		int nv=nbp;
		bb->a=v->p; bb->b=v->p;
		++v;
		while (--nv)
		{
				   if (v->p.x<bb->a.x) bb->a.x=v->p.x;
			else if (v->p.x>bb->b.x) bb->b.x=v->p.x;
				   if (v->p.y<bb->a.y) bb->a.y=v->p.y;
			else if (v->p.y>bb->b.y) bb->b.y=v->p.y;
				   if (v->p.z<bb->a.z) bb->a.z=v->p.z;
			else if (v->p.z>bb->b.z) bb->b.z=v->p.z;
			++v;
		}
	}
	else bb->Clear();
}

