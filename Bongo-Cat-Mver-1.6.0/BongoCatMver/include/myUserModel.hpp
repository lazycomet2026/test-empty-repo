

#pragma once

#include <CubismFramework.hpp>
#include <Model/CubismUserModel.hpp>
#include <ICubismModelSetting.hpp>
#include <Type/csmRectF.hpp>
#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>





/**
 * @brief 用户实际使用的模型的实现类<br>
 *         调用模型生成，功能组件生成，更新处理和渲染。
 *
 */

class LAppTextureManager;


class myUserModel : public Csm::CubismUserModel
{
public:
    /**
     * @brief コンストラクタ   构造函数
     */
    myUserModel();

    /**
     * @brief デストラクタ   析构函数
     *
     */
    virtual ~myUserModel();

    /**
     * @brief model3.jsonが置かれたディレクトリとファイルパスからモデルを生成する  从model3.json所在的目录和文件路径生成模型
     *
     */
    void LoadAssets(const Csm::csmChar* dir, const  Csm::csmChar* fileName);

    /**
     * @brief レンダラを再構築する     重载渲染器
     *
     */
    void ReloadRenderer();

    /**
     * @brief   モデルの更新処理。モデルのパラメータから描画状態を決定する。   模型更新处理。 图纸状态由模型参数确定。
     *
     */
    void Update();

    /**
     * @brief   モデルを描画する処理。モデルを描画する空間のView-Projection行列を渡す。   绘制模型的过程。 通过绘制模型的空间的View-Projection矩阵。
     *
     * @param[in]  matrix  View-Projection行列
     */
    void Draw(Csm::CubismMatrix44& matrix);

    /**
     * @brief   引数で指定したモーションの再生を開始する。   开始播放参数指定的动作
     *
     * @param[in]   group                       モーショングループ名   motion group名称
     * @param[in]   no                          グループ内の番号   group内序号
     * @param[in]   priority                    優先度
     * @param[in]   onFinishedMotionHandler     モーション再生終了時に呼び出されるコールバック関数。NULLの場合、呼び出されない。   动态播放结束时调用的回调函数。 如果为NULL，则不调用。
     * @return                                  開始したモーションの識別番号を返す。個別のモーションが終了したか否かを判定するIsFinished()の引数で使用する。開始できない時は「-1」

     返回开始运动的标识号。 在IsFinished（）的参数中使用，以确定单个运动是否已结束。 无法启动时为“ -1”

     */
    Csm::CubismMotionQueueEntryHandle StartMotion(const Csm::csmChar* group, Csm::csmInt32 no, Csm::csmInt32 priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL);

    /**
     * @brief   开始播放随机选择的动作
     *
     * @param[in]   group                       モーショングループ名   motion group名称
     * @param[in]   priority                    優先度
     * @param[in]   onFinishedMotionHandler     モーション再生終了時に呼び出されるコールバック関数。NULLの場合、呼び出されない。  动态播放结束时调用的回调函数。 如果为NULL，则不调用。
     * @return                                  開始したモーションの識別番号を返す。個別のモーションが終了したか否かを判定するIsFinished()の引数で使用する。開始できない時は「-1」

      返回开始运动的标识号。 在IsFinished（）的参数中使用，以确定单个运动是否已结束。 无法启动时为“ -1”
     */
    Csm::CubismMotionQueueEntryHandle StartRandomMotion(const Csm::csmChar* group, Csm::csmInt32 priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL);

    /**
     * @brief   引数で指定した表情モーションをセットする  设置参数指定的表达式运动
     *
     * @param   expressionID    表情モーションのID
     */
    void SetExpression(const Csm::csmChar* expressionID);

    /**
     * @brief   ランダムに選ばれた表情モーションをセットする   设置随机选择的面部表情动作
     *
     */
    void SetRandomExpression();

    /**
    * @brief   イベントの発火を受け取る  接收事件触发
    *
    */
    virtual void MotionEventFired(const Live2D::Cubism::Framework::csmString& eventValue);

    /**
     * @brief    当たり判定テスト。<br>  命中检测
     *            指定IDの頂点リストから矩形を計算し、座標が矩形範囲内か判定する。  从指定ID的顶点列表中计算出一个矩形，然后确定坐标是否在矩形范围内。
     *
     * @param[in]   hitAreaName     当たり判定をテストする対象のID   测试命中判定的ID
     * @param[in]   x               判定を行うX座標     X坐标判定
     * @param[in]   y               判定を行うY座標     Y坐标判定
     */
    virtual Csm::csmBool HitTest(const Csm::csmChar* hitAreaName, Csm::csmFloat32 x, Csm::csmFloat32 y);


    virtual Csm::ICubismModelSetting* GetModelSetting();
        














    /**
     * @brief   別ターゲットに描画する際に使用するバッファの取得     绘制到另一个目标时获取缓冲区
     */
    Csm::Rendering::CubismOffscreenFrame_OpenGLES2& GetRenderBuffer();

protected:
    /**
     *  @brief  モデルを描画する処理。モデルを描画する空間のView-Projection行列を渡す。
     *绘制模型的过程。 通过绘制模型的空间的View-Projection矩阵。

     */
    void DoDraw();

private:
    /**
     * @brief model3.jsonからモデルを生成する。<br>  从model3.json生成模型
     *         model3.jsonの記述に従ってモデル生成、モーション、物理演算などのコンポーネント生成を行う。
     *      根据model3.json的描述生成组件，例如模型生成，运动和物理操作。
     * @param[in]   setting     ICubismModelSettingのインスタンス
     *
     */
    void SetupModel(Csm::ICubismModelSetting* setting);

    /**
     * @brief OpenGLのテクスチャユニットにテクスチャをロードする   将纹理加载到OpenGL纹理单元中
     *
     */
    void SetupTextures();

    /**
     * @brief   モーションデータをグループ名から一括でロードする。<br>   从组名中批量加载运动数据。
     *           モーションデータの名前は内部でModelSettingから取得する。  运动数据的名称是从ModelSetting内部获得的
     *
     * @param[in]   group  モーションデータのグループ名    motion数据的group名
     */
    void PreloadMotionGroup(const Csm::csmChar* group);

    /**
     * @brief   モーションデータをグループ名から一括で解放する。<br>  立即从group名中释放motion数据。
     *           モーションデータの名前は内部でModelSettingから取得する。    motion数据的名称是从ModelSetting内部获得的
     *
     * @param[in]   group  モーションデータのグループ名   motion数据的group名
     */
    void ReleaseMotionGroup(const Csm::csmChar* group) const;

    /**
    * @brief すべてのモーションデータの解放   释放所有运动数据
    *
    * すべてのモーションデータを解放する。    释放所有motion数据
    */
    void ReleaseMotions();

    /**
    * @brief すべての表情データの解放   释放所有面部表情数据
    *
    * すべての表情データを解放する。   释放所有面部表情数据
    */
    void ReleaseExpressions();

    Csm::ICubismModelSetting* _modelSetting; ///< モデルセッティング情報   Model Setting 的信息
    Csm::csmString _modelHomeDir; ///< モデルセッティングが置かれたディレクトリ  模型设置所在的目录
    Csm::csmFloat32 _userTimeSeconds; ///< デルタ時間の積算値[秒]    增量时间的积分值[秒]
    Csm::csmVector<Csm::CubismIdHandle> _eyeBlinkIds; ///< モデルに設定されたまばたき機能用パラメータID     在模型中设置的眨眼功能参数ID
    Csm::csmVector<Csm::CubismIdHandle> _lipSyncIds; ///< モデルに設定されたリップシンク機能用パラメータID   模型中设置的口型同步功能的参数ID
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _motions; ///< 読み込まれているモーションのリスト    加载motion列表
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _expressions; ///< 読み込まれている表情のリスト      加载表情烈庙
    Csm::csmVector<Csm::csmRectF> _hitArea;
    Csm::csmVector<Csm::csmRectF> _userArea;
    const Csm::CubismId* _idParamAngleX; ///< パラメータID: ParamAngleX      参数id
    const Csm::CubismId* _idParamAngleY; ///< パラメータID: ParamAngleX
    const Csm::CubismId* _idParamAngleZ; ///< パラメータID: ParamAngleX
    const Csm::CubismId* _idParamBodyAngleX; ///< パラメータID: ParamBodyAngleX
    const Csm::CubismId* _idParamEyeBallX; ///< パラメータID: ParamEyeBallX
    const Csm::CubismId* _idParamEyeBallY; ///< パラメータID: ParamEyeBallXY







    LAppTextureManager* _textureManager;
    Csm::Rendering::CubismOffscreenFrame_OpenGLES2  _renderBuffer;   ///< フレームバッファ以外の描画先        帧缓冲？？？
};



